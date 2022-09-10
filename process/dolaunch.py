"""
GPS Visualiser: https://www.gpsvisualizer.com/map_input?allow_export=1&bg_map=google_hybrid&country=gb&form=html&format=google&units=metric
Draw marker details: https://www.gpsvisualizer.com/tutorials/waypoints.html Section 10 for attributes and section 5 for the icons
"""

from zipfile import ZipFile
import math
import csv
import matplotlib.pyplot as plt
from myConfig import apikey
import glob

DUFFVAL = -9999
files = [f for f in glob.glob("*.csv")]
# fn = "050255.csv"
# fn = "051416.csv"
# fn = "052635.csv"
# fn = "054029.csv"

# apikey = "" # defined in myConfig.py
earth_radius = 6372797.56085  # m
one_g = 9.81  # m/s2

if apikey == "":
    print("You need to specify a google maps API key in myConfig.py")
    exit()


def newPos(origin, d, b):
    """
    https://www.igismap.com/formula-to-find-bearing-or-heading-angle-between-two-points-latitude-longitude/#:~:text=Here%20is%20the%20formula%20to,%E2%80%93%20sin%20la1%20*%20sin%20la2)
    Let first point latitude be la1,
    longitude as lo1,
    d be distance,
    R as earth_radius of Earth,
    Ad be the angular distance i.e d/R and
    θ be the bearing,
    Here is the formula to find the second point, when first point, bearing and distance is known:

    latitude of second point = la2 =  asin(sin la1 * cos Ad  + cos la1 * sin Ad * cos θ), and
    longitude  of second point = lo2 = lo1 + atan2(sin θ * sin Ad * cos la1 , cos Ad – sin la1 * sin la2)
    """
    la1, lo1 = origin
    la1 = math.radians(la1)
    lo1 = math.radians(lo1)
    b = math.radians(b)
    Ad = d/earth_radius
    la2 = math.asin(math.sin(la1) * math.cos(Ad) + math.cos(la1) * math.sin(Ad) * math.cos(b))
    lo2 = lo1 + math.atan2(math.sin(b) * math.sin(Ad) * math.cos(la1), math.cos(Ad) - math.sin(la1) * math.sin(la2))
    return (math.degrees(la2), math.degrees(lo2))


def rangeLine(origin, d, b, bd, bs, c, i):
    npoints = ""
    steps = int(((b+bd)-(b-bd))/bs)
    for s in range(steps):
        nb = (b-bd)+(s*bs)
        m = newPos(origin, d, nb)
        npoints += "\t\t\tGV_Draw_Marker({lat:" + str(m[0]) + ",lon:" + str(m[1]) + ",name:'" + str(d) + "m range',desc:'" + str(d) + "m range',color:'" + c + "',icon:'circle'});\n"

    m = newPos(origin, d, b+bd+1*bs)
    npoints += "\t\t\tGV_Draw_Marker({lat:" + str(m[0]) + ",lon:" + str(m[1]) + ",name:'" + str(d) + "m range',desc:'" + str(d) + "m range',color:'" + c + "',icon:'" + i + "'});\n"

    return npoints


def bearing(origin, destination):
    lat1, lng1 = origin
    lat2, lng2 = destination
    """
    // From the esp code
    double gpsBearing(double lat1, double lng1, double lat2, double lng2) {
        // https: // towardsdatascience.com/calculating-the-bearing-between-two-geospatial-coordinates-66203f57e4b4
        double dL = lng2 - lng1
        double X = cos(lat2) * sin(dL)
        double Y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dL)
        double bearing = fmod((atan2(X, Y) * (180 / PI)) + 360, 360)

        return bearing
    }
    """
    lat1 = math.radians(lat1)
    lng1 = math.radians(lng1)
    lat2 = math.radians(lat2)
    lng2 = math.radians(lng2)
    dL = lng2 - lng1
    X = math.cos(lat2)*math.sin(dL)
    Y = math.cos(lat1) * math.sin(lat2) - math.sin(lat1) * math.cos(lat2) * math.cos(dL)
    bearing = math.fmod((math.atan2(X, Y) * (180 / math.pi)) + 360, 360)
    return bearing


def distance(origin, destination):
    """
    Calculate the Haversine distance.

    Parameters
    ----------
    origin : tuple of float
        (lat, long)
    destination : tuple of float
        (lat, long)

    Returns
    -------
    distance_in_km : float

    Examples
    --------
    >>> origin = (48.1372, 11.5756)  # Munich
    >>> destination = (52.5186, 13.4083)  # Berlin
    >>> round(distance(origin, destination), 1)
    504.2
    """
    lat1, lon1 = origin
    lat2, lon2 = destination

    dlat = math.radians(lat2 - lat1)
    dlon = math.radians(lon2 - lon1)
    a = (math.sin(dlat / 2) * math.sin(dlat / 2) +
         math.cos(math.radians(lat1)) * math.cos(math.radians(lat2)) *
         math.sin(dlon / 2) * math.sin(dlon / 2))
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
    d = earth_radius * c

    return d


def processCsvFile(fn):
    # Define the field locations of interest
    emu_alt = 7
    imu_acc = 9
    gps_lat = 20
    gps_lng = 21
    gps_alt = 22
    tvl_spd = 25
    tvl_dst = 26
    tvl_hgt = 27
    det_lnc = 28
    det_cht = 30
    det_lnd = 32
    cht_rsn = 31

    # Define the strings used to detect events
    launchd_str = "## LAUNCH DETECTED"
    chuted_a_str = "## PARACHUTE DEPLOYED (APOGEE)"
    chuted_d_str = "## PARACHUTE DEPLOYED (DISTANCE)"
    landd_str = "## LANDIND DETECTED"

    # define some storage for the key events
    deploy_reason = ""  # The reason for parachute deployment
    start_ms = 0  # the actual logged millis of launch (that we are interested in anyway - the start of the graph)
    start_lat = 0  # the launch position
    start_lng = 0  # the launch position
    start_alt = 0  # the altitude to remove from all altitude values
    launchd_ms = DUFFVAL  # the millis of launch detect
    chuted_ms = DUFFVAL  # the millis of the parachute deployment
    landd_ms = DUFFVAL  # the millis of the detected landing
    end_ms = 0  # the actual logged millis of the landing (that we are interested in - the end of the graph)
    end_lat = 0  # the landing position
    end_lng = 0  # the landing position

    alt_grad_smooth = []
    alt_grad_max = 3

    with open(fn, newline='') as f:
        reader = csv.reader(f)
        rows = list(reader)

    print("##########################################################################################")
    print("File: '" + fn + "' - " + str(len(rows)) + " rows")
    # print(rows[1])

    phase = 0  # 0: prelaunch, 1: pre landing

    grad_yvals = []
    grad_xvals = []

    # First go find all the points calulated by the on-board software
    for i in range(len(rows)-1):
        if i > 2 and not rows[i][0].isnumeric():
            if rows[i][0] == launchd_str:
                launchd_ms = int(rows[i-1][0])
                print("Launch detected at " + str(launchd_ms) + " ms")
            if rows[i][0] == chuted_a_str or rows[i][0] == chuted_d_str:
                chuted_ms = int(rows[i-1][0])
                deploy_reason = rows[i+1][cht_rsn]
                print("Chute deployed at " + str(chuted_ms) + " ms - '" + deploy_reason + "'")
            if rows[i][0] == landd_str:
                landd_ms = int(rows[i-1][0])
                print("Landing detected at " + str(landd_ms) + " ms")
            # print(rows[i][0])

    # Should work out if we can continue with some of these, but full processing needs them all
    if launchd_ms == DUFFVAL:
        print("### Primary event not found - Launch")
        return

    if chuted_ms == DUFFVAL:
        print("### Primary event not found - Parachute deployment")
        return

    if landd_ms == DUFFVAL:
        print("### Primary event not found - Landing")
        return

    # Now we can use those as localiser values
    for i in range(len(rows)-1):
        if i > 2 and rows[i][0].isnumeric():
            ts_alt = (int(rows[i][0]), float(rows[i][emu_alt]))
            alt_grad_smooth.append(ts_alt)

            if len(alt_grad_smooth) > alt_grad_max:
                alt_grad_smooth.pop(0)
                ms_now = int(alt_grad_smooth[alt_grad_max-1][0])
                alt_now = alt_grad_smooth[alt_grad_max-1][1]
                ms_then = alt_grad_smooth[0][1]
                alt_then = alt_grad_smooth[0][1]
                grad = (alt_now - alt_then) / ((ms_now - ms_then)/1000)
                # print(str(ms_now) + ": grad: " + str(grad))
                if start_ms == 0 and ms_now > (launchd_ms - 5000) and grad < -0.1:
                    start_ms = ms_now - 1000
                    start_lat = float(rows[i-alt_grad_max][gps_lat])
                    start_lng = float(rows[i-alt_grad_max][gps_lng])
                    start_alt = float(rows[i-alt_grad_max][emu_alt])
                    print("Real launch around: " + str(start_ms) + " ms, alt: " + str(start_alt) + " m")
                if end_ms == 0 and ms_now > (chuted_ms + 2000) and grad > -0.001:
                    end_ms = ms_now
                    print("Real land around: " + str(end_ms) + " ms")
                if ms_now > 36500 and ms_now < 55000:
                    grad_xvals.append(ms_now)
                    grad_yvals.append(grad)

    graph_launch_x = [(launchd_ms-start_ms)/1000]
    graph_launch_y = [0]
    graph_land_x = [(landd_ms-start_ms)/1000]
    graph_land_y = [0]
    graph_chute_x = [(chuted_ms-start_ms)/1000]
    graph_chute_y = [0]

    graph_millis = []
    graph_emu_alt = []
    graph_gps_alt = []
    graph_imu_acc = []
    graph_tvl_spd = []
    graph_tvl_rspd = []  # recalculated data
    graph_tvl_rdst = []

    max_lat = DUFFVAL
    min_lat = -DUFFVAL
    max_lng = DUFFVAL
    min_lng = -DUFFVAL
    last_lat = DUFFVAL
    last_lng = DUFFVAL
    last_alt = DUFFVAL
    last_ms = DUFFVAL

    # output file for geo thing
    orows = []
    title = "Launch " + fn
    center = ""
    points = ""

    desc_chute = False
    for i in range(len(rows)-1):
        if i > 2 and rows[i][0].isnumeric():
            millis = int(rows[i][0])
            if millis >= start_ms and millis <= end_ms:
                graph_millis.append((millis-start_ms)/1000)
                graph_emu_alt.append(float(rows[i][emu_alt]) - start_alt)
                graph_gps_alt.append(float(rows[i][gps_alt]) - start_alt)
                graph_imu_acc.append(float(rows[i][imu_acc])/one_g)

                # Generate speed graph from raw data
                tvsp = rows[i][tvl_spd]
                if tvsp == "":
                    tvsp = "0.0"
                graph_tvl_spd.append(float(tvsp))

                # recalulate distance and speeds
                lat = float(rows[i][gps_lat])
                lng = float(rows[i][gps_lng])
                end_lat = lat
                end_lng = lng
                alt = max(0, float(rows[i][emu_alt]))
                ts = int(rows[i][0])

                if last_alt > DUFFVAL:
                    xy_dst = distance((last_lat, last_lng), (lat, lng))
                    tot_dst = distance((start_lat, start_lng), (lat, lng))
                    # print("start lat: " + str(start_lat))
                    # print("start lng: " + str(start_lng))
                    # print("lat: " + str(lat))
                    # print("lng " + str(lng))
                    # print("tot dst: " + str(tot_dst))
                    graph_tvl_rdst.append(xy_dst)
                    zz_dst = alt - last_alt
                    lin_dst = math.sqrt(pow(xy_dst, 2) + pow(zz_dst, 2))
                    lin_spd = lin_dst / ((ts - last_ts)/1000)
                    graph_tvl_rspd.append(lin_spd)

                    desc = "alt: " + ("{:.2f}".format(alt)) + "m, spd: " + ("{:.2f}".format(lin_spd)) + "m/s, dst: " + ("{:.2f}".format(tot_dst)) + "m"
                    pin_color = "green"
                    pin_icon = "circle"
                    name = "T+" + str((int(rows[i][0])-start_ms)/1000) + "s"
                    if int(rows[i][0]) >= chuted_ms and desc_chute == False:
                        desc_chute = True
                        name = "Parachute deployed"
                        desc += ", Chute deployed: " + deploy_reason
                        pin_color = "orange"
                        pin_icon = "googlemini"

                    orows.append([int(rows[i][0])-start_ms, '"' + desc + '"', lat, lng])

                    # GV_Draw_Marker({lat:51.3840161,lon:-0.4787488,alt:12.28,name:'35634',desc:'alt: 12.28, hPa: 1023.71, spd: 0.23, Chute: \'\'',color:'',icon:''});
                    points += "\t\t\tGV_Draw_Marker({lat:" + str(lat) + ",lon:" + str(lng) + ",alt:" + str(alt) + ",name:'" + name + "',desc:'" + str(desc) + "',color:'" + pin_color + "',icon:'" + pin_icon + "'});\n"

                else:
                    orows.append(["name", "description", "latitude", "longitude"])
                    graph_tvl_rdst.append(0)
                    graph_tvl_rspd.append(0)

                max_lat = max(lat, max_lat)
                max_lng = max(lng, max_lng)
                min_lat = min(lat, min_lat)
                min_lng = min(lng, min_lng)
                last_lat = lat
                last_lng = lng
                last_alt = alt
                last_ts = ts

    fn = fn.replace(".csv", "")

    flight_ms = end_ms - start_ms
    flight_bearing = bearing((start_lat, start_lng), (end_lat, end_lng))

    desc = "Flight time: " + "{:.2f}".format(flight_ms/1000) + "s<br>"
    desc += "Flight distance: " + "{:.2f}".format(tot_dst) + "m<br>"
    desc += "Chute deployed: \\'" + deploy_reason + "\\'"
    flag = newPos((start_lat, start_lng), tot_dst+1, flight_bearing)
    points = "\t\t\tGV_Draw_Marker({lat:" + str(flag[0]) + ",lon:" + str(flag[1]) + ",name:'Flight Details',desc:'" + desc + "', color:'#fc0',icon:'airport'});\n" + points

    bd = 10
    bs = 1
    points = rangeLine((start_lat, start_lng), 55, flight_bearing, bd, bs, "#f99", "circle") + points
    points = rangeLine((start_lat, start_lng), 65, flight_bearing, bd, bs, "#fc0", "circle") + points
    points = rangeLine((start_lat, start_lng), 70, flight_bearing, bd, bs, "#9c0", "star") + points
    points = rangeLine((start_lat, start_lng), 75, flight_bearing, bd, bs, "#fc0", "circle") + points
    points = rangeLine((start_lat, start_lng), 85, flight_bearing, bd, bs, "#f99", "circle") + points

    # m65 = newPos((start_lat, start_lng), 65, flight_bearing)
    # m70 = newPos((start_lat, start_lng), 70, flight_bearing)
    # m75 = newPos((start_lat, start_lng), 75, flight_bearing)
    # points = "\t\t\tGV_Draw_Marker({lat:" + str(m65[0]) + ",lon:" + str(m65[1]) + ",name:'65m range',desc:'70m range',color:'#fc0',icon:'circle'});\n" + points
    # points = "\t\t\tGV_Draw_Marker({lat:" + str(m70[0]) + ",lon:" + str(m70[1]) + ",name:'70m range',desc:'70m range',color:'#9c0',icon:'star'});\n" + points
    # points = "\t\t\tGV_Draw_Marker({lat:" + str(m75[0]) + ",lon:" + str(m75[1]) + ",name:'75m range',desc:'70m range',color:'#fc0',icon:'circle'});\n" + points
    # # write out the geodata
    # ofp = open(fn+".geodata.csv", 'w')
    # for orow in orows:
    #     ofp.write(','.join(str(col) for col in orow) + "\n")
    # ofp.close()

    print("### Creating launch map")
    # open text file in read mode
    templatefp = open("geo_template.html", "r")
    template = templatefp.read()
    templatefp.close()

    # [51.38356695, -0.4790812]
    center = "[" + str(min_lat + (max_lat-min_lat)/2) + ", " + str(min_lng + (max_lng-min_lng)/2) + "]"
    template = template.replace("[[APIKEY]]", apikey)
    template = template.replace("[[TITLE]]", title)
    template = template.replace("[[CENTER]]", center)
    template = template.replace("[[POINTS]]", points)
    ofp = open(fn+'.track.html', 'w')
    ofp.write(template)
    ofp.close()

    print("### Creating altitude chart")
    # Plot altitude graphs
    fig = plt.gcf()
    fig.set_size_inches(19.2, 10.8)
    plt.title("Flight Altitude")
    plt.minorticks_on()
    plt.grid(visible=True, axis='y', which='major', color='green', linestyle="-", linewidth=1)
    plt.grid(visible=True, axis='y', which='minor', color='#ddd', linestyle="--", linewidth=1)
    plt.grid(visible=True, axis='x', which='major', color='#ccc', linestyle="-", linewidth=1)
    plt.grid(visible=True, axis='x', which='minor', color='#ddd', linestyle="--", linewidth=0.5)
    plt.xlabel("Flight time (Seconds)")
    plt.ylabel("Altitude (Metres)")
    plt.scatter(graph_millis, graph_imu_acc, label="g-force", color="#6f6")
    plt.scatter(graph_millis, graph_emu_alt, label="Barometric Altitude")
    plt.scatter(graph_millis, graph_gps_alt, label="GPS Altitude", color="#999")

    if launchd_ms >= start_ms and launchd_ms <= end_ms:
        plt.scatter(graph_launch_x, graph_launch_y, label="launch detected", color="red")

    if chuted_ms >= start_ms and chuted_ms <= end_ms:
        plt.scatter(graph_chute_x, graph_chute_y, label="chute deployed", color="orange")

    if landd_ms >= start_ms and landd_ms <= end_ms:
        plt.scatter(graph_land_x, graph_land_y, label="landing detected", color="green")

    plt.legend()
    plt.savefig(fn+'.altitude.png', dpi=300)
    # plt.show()

    print("### Creating speed chart")
    # Plot speed graphs
    fig.clear()
    plt.title("Flight Speed")
    plt.minorticks_on()
    plt.grid(visible=True, axis='y', which='major', color='green', linestyle="-", linewidth=1)
    plt.grid(visible=True, axis='y', which='minor', color='#ddd', linestyle="--", linewidth=1)
    plt.grid(visible=True, axis='x', which='major', color='#ccc', linestyle="-", linewidth=1)
    plt.grid(visible=True, axis='x', which='minor', color='#ddd', linestyle="--", linewidth=0.5)
    plt.xlabel("Flight time (Seconds)")
    plt.ylabel("Linear Speed (Metres/Second)")
    plt.scatter(graph_millis, graph_tvl_spd, label="GPS Speed", color="#9c0")
    plt.scatter(graph_millis, graph_tvl_rspd, label="Recalc Speed", color="#090")

    if launchd_ms >= start_ms and launchd_ms <= end_ms:
        plt.scatter(graph_launch_x, graph_launch_y, label="launch detected", color="red")

    if chuted_ms >= start_ms and chuted_ms <= end_ms:
        plt.scatter(graph_chute_x, graph_chute_y, label="chute deployed", color="orange")

    if landd_ms >= start_ms and landd_ms <= end_ms:
        plt.scatter(graph_land_x, graph_land_y, label="landing detected", color="green")

    plt.legend()
    plt.savefig(fn+'.speed.png', dpi=300)
    # plt.show()

    print("### Creating zip file")
    with ZipFile(fn+'.zip', 'w') as zipObj2:
        # Add multiple files to the zip
        zipObj2.write(fn+'.csv')
        zipObj2.write(fn+'.altitude.png')
        zipObj2.write(fn+'.speed.png')
        zipObj2.write(fn+'.track.html')

    print("Flight time: " + "{:.2f}".format(flight_ms/1000) + "s")
    print("Flight distance: " + "{:.2f}".format(tot_dst) + "m")
    print("Flight bearing: " + "{:03.2f}".format(flight_bearing) + "°")


for fn in files:
    processCsvFile(fn)
