var basePos = [markerArray[0]._latlng.lat, markerArray[0]._latlng.lng];
var gwMarkerIcon = L.icon({
    iconUrl: "/resources/TTNraindropJPM45px.png",
    shadowUrl: "/resources/marker-shadow.png",
    iconSize:     [45, 45], // size of the icon
    shadowSize:   [46, 46], // size of the shadow
    iconAnchor:   [22, 45], // point of the icon which will correspond to marker's location
    shadowAnchor: [16, 46],  // the same for the shadow
    popupAnchor:  [23, 25] // point from which the popup should open relative to the iconAnchor
});
marker = L.marker(basePos, {icon: gwMarkerIcon}).addTo(map);
marker.desc = "B827EBFFFE989A15<br />show only this gateway's coverage as: <br><a href=\"?gateway=B827EBFFFE989A15&type=radials\">radials</a><br><a href=\"?gateway=B827EBFFFE989A15&type=points\">points</a><br><a href=\"?gateway=B827EBFFFE989A15&type=radar\">radar</a><br><a href=\"?gateway=B827EBFFFE989A15&type=alpha\">alpha shape</a><br>";
oms.addMarker(marker);

markerArray.forEach( e => { if (basePos[0] != e._latlng.lat && basePos[1] != e._latlng.lng) {
    markerOptions = {
        radius: 10,
        color: "red",
        fillColor: "red",
        opacity: 0.1,
        weight: 2
    };
    marker = L.polyline([basePos,[e._latlng.lat, e._latlng.lng]], markerOptions);
    //marker.bindPopup("2019-07-29 09:42:16<br /><b>Node:</b> ttgov2no1<br /><b>Received by gateway:</b> <br />B827EBFFFE989A15<br /><b>Location accuracy:</b> 9.65<br /><b>Packet id:</b> 8364901<br /><b>RSSI:</b> -75.00<br /><b>SNR:</b> 12.80<br /><b>DR:</b> SF9BW125<br /><b>Distance:</b> 14.01m<br /><b>Altitude: </b>71.5m");
    markerArray.push(marker);
} } )

// TODO: add just the new group
var group = L.featureGroup(markerArray).addTo(map);
map.fitBounds(group.getBounds());