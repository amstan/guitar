#!/usr/bin/env python3
import periphery
import struct

DEFAULT_I2C_ADDRESS = 0x1e

class ECCommError(Exception):
	pass

class Fret(object):
	def __init__(self, i2c, i2c_address = DEFAULT_I2C_ADDRESS):
		self.i2c = i2c
		self._i2c_address = i2c_address

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
		#raws = self.command(0x10, expected_length=6*2)
		#lsbs=raws[0::2]
		#msbs=raws[1::2]

		#return tuple(msb*256 + lsb for msb, lsb in zip(msbs, lsbs))
		pass

	def set_led(self, i, r, g, b):
		self.command(0x0028, [0,7, i, r,g,b], expected_length = 0)

	def set_leds(self, data):
		self.command(0x0027, data, expected_length = 0)

def demo(fret):
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
	colors=colors[::2] #faster
	#colors = [[b,b,b] for r,g,b in colors] #white glow

	def fix_gamma(x):
		return int(((x / 255) ** 0.7) * 255)
	colors = [[fix_gamma(ch) for ch in color] for color in colors] #lower brightness

	colors = [[r//10,g//10,b//10] for r,g,b in colors] #lower brightness

	print("Watch the pretty colors!")
	while True:
		for j in range(len(colors)):
			c=rotate(colors,len(colors) - j - 1)[::len(colors)//(6*2)]
			#for g in range(20):
				#what if we had more frets(20x slower), was doing touch(2x slower), but increased the i2c speed(10x faster) too
			fret.set_leds(sum(c,[])[:18])
			#for g in range(20):
				#blink test to show the fps
				#fret.set_leds([0xff] * 18)

if __name__=="__main__":
	i2c = periphery.I2C("/dev/i2c-0")
	fret = Fret(i2c)
	demo(fret)
