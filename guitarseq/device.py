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
		max_x = 2048
		interval = max_x / (string_count + 1)
		string_locations = [int((i + 1) * interval) for i in range(string_count)]
		string_locations.reverse()
		print("String positions:", string_locations)

		for packet in self.read_packet_iter():
			if not any((packet["press"], packet["release"], packet["move"])):
				continue

			if packet["id"] in range(finger_offset, finger_offset + finger_count):
				packet["id"] -= finger_offset

			#print(
				#packet["id"],
				#hex(packet["status"]),
				#packet["x"],
				##packet["y"],
				##packet["area"],
				#packet["amplitude"],

				#["","press"][packet["press"]],
				#["","move"][packet["move"]],
				#["","release"][packet["release"]],

				#sep = "\t",
			#)

			if packet["id"] >= 10:
				continue

			if previous_xs[packet["id"]] != None:
				micro_movement = range(*sorted((previous_xs[packet["id"]], packet["x"])))
				for string_id, string_location in enumerate(string_locations):
					if string_location in micro_movement:
						yield({
							"string_id": string_id,
							"velocity": packet["amplitude"]*0.75,
						})

			if packet["release"]:
				previous_xs[packet["id"]] = None
			else:
				previous_xs[packet["id"]] = packet["x"]

if __name__=="__main__":
	d=Device()
	for event in d.process_strings():
		#print(event)
		pass

