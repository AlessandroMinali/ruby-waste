require 'nokogiri'

def distance(lat, lng, lat0, lng0)
  deg2rad = 0.017453292519943295
  lat *= deg2rad
  lng *= deg2rad
  lat0 *= deg2rad
  lng0 *= deg2rad # SIMD!
  a = (
    (1 - Math.cos(lat0 - lat)) +
    (1 - Math.cos(lng0 - lng)) * Math.cos(lat) * Math.cos(lat0)
  ) / 2.0

  12_742 * Math.asin(Math.sqrt(a))
end

def program
  p_start = Time.now
  doc = Nokogiri::XML(File.open("/Users/alessandro/Desktop/gpx/CompleteBajaDivide-Reduced_V1.3(preferred).gpx"))
  points = []
  raw_points = doc.css('trkpt')
  raw_points.each do |point|
    points << [point.xpath('@lat').text.to_f, point.xpath('@lon').text.to_f, point.css('ele').text.to_f.round(1)]
  end
  p_end = Time.now - p_start

  total = 0
  waypoints = []
  d_start = Time.now
  points.each.with_index(1) do |point, index|
    break unless points[index]

    point_next = points[index]
    d = distance(point[0], point[1], point_next[0], point_next[1]) # tight loop this? SIMD + unroll(?)
    total += d

    if total >= 80
      waypoints << [point_next[0], point_next[1]]
      total = 0
    end
  end
  d_end = Time.now - d_start
  return p_end, d_end, waypoints.count
end

waypoints = 0
pt = 999
dt = 999
100.times do |n|
  p_end, d_end, wp = program()
  waypoints += wp
  pt = p_end < pt ? p_end : pt
  dt = d_end < dt ? d_end : dt
end

puts "Waypoints: #{waypoints}"
puts "Parse time:\t#{pt}\t| Parse point/second:\t#{(10_000/pt)}"
puts "Waypoints time:\t#{dt}\t| Waypoint/second:\t#{(33/dt)}"

# Waypoints: 3300
# Parse time: 0.769232  | Parse point/second: 12999.97920003328
# Waypoints time: 0.007707  | Waypoint/second:  4281.821720513818