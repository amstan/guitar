#!/usr/bin/env python3
import collections
import periphery
import struct
import pprint

class ECCommError(Exception):
	pass

def format_uuid(uuid):
	return "(" + ', '.join("0x%08x" % x for x in uuid) + ")"

STARTUP_I2C_ADDRESS = 0x1e
UNKNOWN_DEVICE_I2C_ADDRESS = 0x1f
STAGING_I2C_ADDRESS = 0x1d

class Fret(object):
	def __init__(self, i2c, i2c_address = STARTUP_I2C_ADDRESS):
		self.i2c = i2c
		self._i2c_address = i2c_address
		self.description = ""

		# Make sure to clean the device buffer before trying to speak
		self.flush()

		self.hello()

	def flush(self):
		# A read I2C_MAX_HOST_PACKET_SIZE large
		self.i2c.transfer(self._i2c_address, [periphery.I2C.Message([0x00] * 128, read=True)])

	def command(self, command, args=[], expected_length=None):
		try:
			msg = [0x03, 0x00, command & 0xff, command >> 8, 0x00, 0x00, len(args) & 0xff, len(args) >> 8] + list(args)
			checksum = (0x100-sum(msg)%0x100)%0x100
			msg[1]=checksum

			tx = periphery.I2C.Message([0xda] + msg)

			if expected_length is not None:
				expected_length += 8 # reply packet overhead
				read = periphery.I2C.Message([0x00] * (2 + expected_length), read=True)
				self.i2c.transfer(self._i2c_address, [tx, read])

				_, length, *reply_packet = read.data

				if length != expected_length:
					raise ECCommError("received_length %d != expected_length %d" % (length, expected_length))
			else:
				read1 = periphery.I2C.Message([0x00] * 2, read=True)
				self.i2c.transfer(self._i2c_address, [tx, read1])

				_, length = read1.data

				read2 = periphery.I2C.Message([0x00] * length, read=True)
				self.i2c.transfer(self._i2c_address, [read2])
				reply_packet = read2.data

			version, checksum, response_low, response_high, data_len_low, data_len_high, _reserved_low, _reserved_high, *reply_data = reply_packet

			if version != 0x03:
				raise ECCommError("Received wrong version %02x" % version)

			#TODO checksum

			response = (response_high << 8) + response_low
			if response != 0x00:
				raise ECCommError("Bad command response: 0x%02x" % response)

			data_len = (data_len_high << 8) + data_len_low
			if len(reply_data) > data_len:
				assert(reply_data[data_len] == 0xec)
			reply_data = reply_data[:data_len]

			return bytes(reply_data)
		except:
			self.flush()
			raise

	def hello(self, payload = 0x25):
		hello_struct = struct.Struct("<I")
		to_send = hello_struct.pack(payload)
		reply = self.command(0x0001, to_send, expected_length = hello_struct.size)
		reply, = hello_struct.unpack(reply)
		if reply != (payload + 0x01020304) & 0xffffffff:
			raise ECCommError("Hello failed")

	@property
	def version(self):
		version_struct = struct.Struct("<32s32s32sI")

		version_reply = bytes(self.command(0x0002, expected_length = version_struct.size))
		ro_version, rw_version, _, copy = version_struct.unpack(version_reply)

		ro_version = ro_version.partition(b"\x00")[0].decode("utf-8")
		rw_version = rw_version.partition(b"\x00")[0].decode("utf-8")
		copy = ["UNKNOWN", "RO", "RW"][copy]

		build_info = bytes(self.command(0x0004)).partition(b"\x00")[0].decode("utf-8")

		return {
			"RO version": ro_version,
			"RW version": rw_version,
			"Firmware copy": copy,
			"Build info": build_info,
		}

	@property
	def uuid(self):
		uuid_struct = struct.Struct("<III")
		return uuid_struct.unpack(bytes(self.command(0x0031, expected_length=uuid_struct.size)))

	def change_i2c_address(self, new_address, uuid = None):
		if uuid is not None:
			to_send = struct.pack("<bIII", new_address << 1, *uuid)
		else:
			to_send = struct.pack("<b", new_address << 1)

		try:
			self.command(0x0032, to_send, expected_length = 0)
		except Exception:
			#Our reply probably couldn't get processed, since we're asking the wrong device now
			pass
		self._i2c_address = new_address

		self.flush()
		self.hello()

	@property
	def i2c_address(self):
		return self._i2c_address
	@i2c_address.setter
	def i2c_address(self, new_address):
		self.change_i2c_address(new_address)

	@property
	def touch(self):
		reply = self.command(0x0034, [])

		touch_struct = struct.Struct("<" + "H" * (len(reply) // 2))
		return touch_struct.unpack(reply)

	def set_led(self, i, r, g, b):
		self.command(0x0028, [0,7, i, r,g,b], expected_length = 0)

	def set_leds(self, data):
		self.command(0x0027, data, expected_length = 0)

	def i2c_led_demo(self, touch_source=None):
		def rotate(l,n):
			return l[n:] + l[:n]

		colors = (
			[[255-x,0,x] for x in range(255)] +
			[[0,x,255-x] for x in range(255)] +
			[[x,255-x,0] for x in range(255)]
		)
		#colors = (
			#[[255,0,255-x] for x in range(255)] +
			#[[255,x,0] for x in range(255)] +
			#[[255-x,255,0] for x in range(255)] +
			#[[0,255,x] for x in range(255)] +
			#[[0,255-x,255] for x in range(255)] +
			#[[x,0,255] for x in range(255)]
		#)
		colors=colors[::8] #faster
		#colors = [[b,b,b] for r,g,b in colors] #white glow

		def fix_gamma(x):
			return int(((x / 255) ** 0.7) * 255)
		colors = [[fix_gamma(ch) for ch in color] for color in colors] #lower brightness

		if touch_source is None:
			colors = [[r//10,g//10,b//10] for r,g,b in colors] #lower brightness
			pass

		print("Watch the pretty colors!")
		while True:
			for j in range(len(colors)):
				c = rotate(colors,len(colors) - j - 1)[::len(colors)//(6)]
				#for g in range(20):
					#what if we had more selfs(20x slower), was doing touch(2x slower), but increased the i2c speed(10x faster) too

				if touch_source is not None:
					grayscale = " .:-=+*#%@"
					touch = touch_source.touch
					for t in touch:
						print ("%27s" % (grayscale[t % len(grayscale)] + grayscale[-1] * int(t / len(grayscale)),), end="")
					print()

					c = [[(ch * t // 256) for ch in color] for color, t in zip(c, touch)]

				self.set_leds(sum(c,[])[:18])


	def __repr__(self):
		return super().__repr__()[:-1] + ", i2c 0x%02x %r>" % (self._i2c_address, self.description)

	def __str__(self):
		return "Fret at address 0x%02x\nDescription %r\nUUID %s\n%s\n" % (
			self._i2c_address,
			self.description,
			format_uuid(self.uuid),
			pprint.pformat(self.version)
		)

	def colorize_address(self):
		led_data = []
		for i in reversed(range(6)):
			led_data += ([1],[20])[int(bool(self.i2c_address & (1 << i)))] * 3
		self.set_leds(led_data)


class FretCollection(collections.UserDict):
	def __init__(self, i2c):
		self.i2c = i2c
		super().__init__()

	def enumerate(self, assignment, extra_scan_addresses=[]):
		assignment = assignment.copy()

		# Put everything we know so far into the assignment too,
		# since we have to start from scratch anyway
		for fret in list(self.values()):
			# TODO fret.uuid here might be bad
			# perhaps make uuid cached
			if fret.uuid not in assignment:
				assignment[fret.uuid] = (fret.i2c_address, fret.description)
			del self[fret.i2c_address]

		self.colorize_addresses()

		scan_addresses = [a for a, _ in assignment.values()] + [UNKNOWN_DEVICE_I2C_ADDRESS, STAGING_I2C_ADDRESS] + extra_scan_addresses
		print("Moving all devices from random addresses [%s] to the startup address 0x%02x:" % (', '.join("0x%02x" % a for a in scan_addresses), STARTUP_I2C_ADDRESS))
		for address in scan_addresses:
			print("Scanning address 0x%02x..." % address)
			while True:
				try:
					f = Fret(i2c, i2c_address = address)
				except periphery.i2c.I2CError:
					break

				found_uuid = f.uuid
				print(format_uuid(f.uuid))
				f.change_i2c_address(STARTUP_I2C_ADDRESS, found_uuid)

		self.colorize_addresses()

		print("\nScanning at startup address 0x%02x..." % (STARTUP_I2C_ADDRESS))
		while True:
			try:
				# Try to enumerate at the search location
				f = Fret(i2c, i2c_address = STARTUP_I2C_ADDRESS)
			except periphery.i2c.I2CError:
				break

			# If we find one, check its uuid and change it's address to the proper one
			found_uuid = f.uuid

			try:
				# Grab stuff from the database for this uuid
				address, description = assignment.pop(found_uuid)
			except KeyError:
				# We might not know about it
				print("Unknown device!")
				address = UNKNOWN_DEVICE_I2C_ADDRESS
				f.description = "Unknown"
				f.change_i2c_address(UNKNOWN_DEVICE_I2C_ADDRESS, found_uuid)
			else:
				# If we know what it is, save it in the database
				f.description = description
				self[address] = f
				f.change_i2c_address(address, found_uuid)
				f.colorize_address()

			print(f)

		self.colorize_addresses()

		for uuid, (address, description) in assignment.items():
			print("Could not find %s %r who was supposed to go at address 0x%02x" % (format_uuid(uuid), description, address))

	def colorize_addresses(self):
		try:
			startup_frets = Fret(i2c, i2c_address = STARTUP_I2C_ADDRESS)
		except periphery.i2c.I2CError:
			pass
		else:
			startup_frets.set_leds([40, 1, 1,  5, 20, 5] + [1, 2, 1] * 4)

		try:
			unknown_frets = Fret(i2c, i2c_address = UNKNOWN_DEVICE_I2C_ADDRESS)
		except periphery.i2c.I2CError:
			pass
		else:
			unknown_frets.set_leds([40, 1, 1,  20, 10, 1] + [2, 1, 1] * 4)

		try:
			staging_frets = Fret(i2c, i2c_address = STAGING_I2C_ADDRESS)
		except periphery.i2c.I2CError:
			pass
		else:
			staging_frets.set_leds([40, 1, 1,  1, 10, 10] + [1, 2, 2] * 4)

		for fret in self.values():
			fret.colorize_address()

	def __str__(self):
		return '\n'.join("0x%02x: %r" % item for item in self.items())
	__repr__ = __str__


if __name__=="__main__":
	i2c = periphery.I2C("/dev/i2c-0")
	collection = FretCollection(i2c)
	device_list = {
		(0x00430043, 0x46335717, 0x20333539): (0x20, "fret with pins"),
		(0x00230024, 0x42365712, 0x32353530): (0x21, "discovery 1"),
		(0x0033003c, 0x42365714, 0x32353530): (0x22, "discovery 2"),
	}
	collection.enumerate(device_list)
	if 0x20 in collection:
		fret = collection[0x20]
	disco = collection[0x21]
	disco.i2c_led_demo(fret)
