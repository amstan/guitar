#!/usr/bin/env python3

import serial
import sys

class Device(object):
	def __init__(self):
		self.serial = serial.Serial("/dev/serial/by-id/usb-HyperTriange_Guitar-if00", timeout=0.1)

	def sync(self, packet):
		start_magic = b"data:\n"

		print("Sync")

		try:
			print(packet.decode("ascii"), end="", flush=True)

			while (packet[-len(start_magic):] != start_magic):
				c = self.serial.read(1)
				packet += c
				print(c.decode("ascii"), end="", flush=True)
			print("\nDone sync")
		except UnicodeDecodeError:
			print("\nFalse Alarm")
			self.serial.flushInput()

	def poll(self):
		while 1:
			packet = self.serial.read(8)

			if not len(packet):
				continue

			if len(packet) != 8 or b"Cons" in packet:
				self.sync(packet)
				continue

			packet = {
				"id": packet[0],

				"status": packet[1],
				"ungrip":  bool(packet[1] & (1<<0)),
				"supress": bool(packet[1] & (1<<1)),
				"amp":     bool(packet[1] & (1<<2)),
				"vector":  bool(packet[1] & (1<<3)),
				"move":    bool(packet[1] & (1<<4)),
				"release": bool(packet[1] & (1<<5)),
				"press":   bool(packet[1] & (1<<6)),
				"detect":  bool(packet[1] & (1<<7)),

				"x": (packet[2]<<8) + (packet[4] & 0xf0),
				"y": (packet[3]<<8) + ((packet[4] & 0xf0)<<4),
				"area": packet[5],
				"amplitude": packet[6],
				"vector1": (packet[7] & 0xf0) >> 4,
				"vector2": (packet[7] & 0x0f),
			}

			packet["x"]//=16
			packet["y"]//=16

			yield ("strings", packet)

	def process_strings(self):
		finger_count = 10
		finger_offset = 2
		previous_xs = [None] * 10
		string_count = 6
		max_x = 2048
		interval = max_x / (string_count + 1)
		string_locations = [int((i + 1) * interval) for i in range(string_count)]
		print("String positions:", string_locations)

		for any_packet in self.poll():
			packet=any_packet[1]

			if not any((packet["press"], packet["release"], packet["move"])):
				continue


			if packet["id"] in range(finger_offset, finger_offset + finger_count):
				packet["id"] -= finger_offset

			print(
				packet["id"],
				hex(packet["status"]),
				packet["x"],
				#packet["y"],
				#packet["area"],
				packet["amplitude"],

				["","press"][packet["press"]],
				["","move"][packet["move"]],
				["","release"][packet["release"]],

				sep = "\t",
			)

			if packet["id"] >= 10:
				continue

			if previous_xs[packet["id"]] != None:
				micro_movement = range(*sorted((previous_xs[packet["id"]], packet["x"])))
				for string_id, string_location in enumerate(string_locations):
					if string_location in micro_movement:
						yield({
							"string_id": string_id,
							"velocity": packet["amplitude"],
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

