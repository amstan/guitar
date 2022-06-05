#!/usr/bin/env python3

import sys

class Device(object):
	def __init__(self, read_packet_iter):
		self.read_packet_iter = read_packet_iter

	def process_strings(self):
		finger_count = 10
		finger_offset = 2
		previous_xs = [None] * 10
		string_count = 6
		max_x = 2040
		interval = max_x / (string_count + 1)
		string_locations = [
			(int((i + 1) * interval - interval / 2),
			int((i + 1) * interval + interval / 2))
				for i in range(6)]
		strings_for_fingers = {f:set() for f in range(finger_count)}
		print("String positions:", string_locations)

		for packet in self.read_packet_iter():
			if not any((packet["press"], packet["release"], packet["move"])):
				continue

			if packet["id"] in range(finger_offset, finger_offset + finger_count):
				finger_id = packet["id"] - finger_offset
			else:
				continue

			#print(
				#finger_id,
				#hex(packet["status"]),
				#packet["x"],
				#packet["y"],
				#packet["area"],
				#packet["amplitude"],

				#["","press"][packet["press"]],
				#["","move"][packet["move"]],
				#["","release"][packet["release"]],

				#sep = "\t",
			#)

			packet["x"] = max_x - packet["x"] - 1

			if finger_id >= 10:
				continue

			def strings_crossed_by_range(r):
				r.sort()
				crossing = r[0] < string_locations[0][0]
				if r[1] < string_locations[0][0]:
					# everything's happening before the first string anyway
					return set()
				strings = set()
				for string_id, (string_start, string_stop) in enumerate(string_locations):
					for range_endpoint in r:
						if range_endpoint >= string_start and range_endpoint < string_stop:
							crossing = not crossing
						if crossing:
							strings.add(string_id)

				print(r,strings)
				return strings

				#string_locations
				#strings_crossed_by_range([1500,1100])
				#strings_crossed_by_range([200,500])

			if previous_xs[finger_id] is None:
				previous_xs[finger_id] = packet["x"]

			new_touched_strings = strings_crossed_by_range([previous_xs[finger_id], packet["x"]])

			for muting_string in new_touched_strings:
				yield((1,"ps%d" % muting_string))

			if packet["release"]:
				new_touched_strings = set()

			#print(packet["release"],new_touched_strings, strings_for_fingers)

			for playing_string in strings_for_fingers[finger_id] - new_touched_strings:
				yield((1,"rs%dv%03d" % (playing_string, packet["amplitude"]*0.75)))


			if packet["release"]:
				previous_xs[finger_id] = None
				strings_for_fingers[finger_id] = set()
			else:
				previous_xs[finger_id] = packet["x"]
				strings_for_fingers[finger_id] = new_touched_strings

if __name__=="__main__":
	d=Device()
	for event in d.process_strings():
		#print(event)
		pass

