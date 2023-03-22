require 'nokogiri'

def distance(lat, lng, lat0, lng0)
  deg2rad = 0.017453292519943295
  lat *= deg2rad
  lng *= deg2rad
  lat0 *= deg2rad
  lng0 *= deg2rad
  a = (
    (1 - Math.cos(lat0 - lat)) +
    (1 - Math.cos(lng0 - lng)) * Math.cos(lat) * Math.cos(lat0)
  ) / 2.0

  12_742 * Math.asin(Math.sqrt(a))
end

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
  d = distance(point[0], point[1], point_next[0], point_next[1])
  total += d

  if total >= 80
    waypoints << [point_next[0], point_next[1]]
    total = 0
  end
end
d_end = Time.now - d_start

puts "Waypoints: #{waypoints.count}"
waypoints.each_with_index do |i, index|
  puts "<wpt lon='#{i[1]}' lat='#{i[0]}'>"
  puts "  <name>#{(index + 1) * 80} km</name>"
  puts "  <desc>#{(0.621371 * (index + 1) * 80).round(1)} mi</desc>"
  puts "</wpt>"
end