from periphery import *
import struct

DEFAULT_I2C_ADDRESS = 0x25

def _process_key(key, size):
	if type(key) is int:
		start = key
		length = 1
		single = True
	else:
		slice_start, slice_stop, slice_step = key.indices(size)
		if slice_step != 1:
			raise AttributeError("Can't have non 1 step.")

		start = slice_start
		length = slice_stop - slice_start
		single = False
	return start, length, single

class FretMemory(object):
	MAXMEM = 0xffffffff
	TRANSFER_BLOCK_SIZE = 16
	_uint32_fmt = "<I"

	def __init__(self, fret):
		self._fret = fret

	def __getitem__(self, key):
		location, length, single = _process_key(key, self.MAXMEM)

		assert(length != 1)

		args = (list(struct.pack(self._uint32_fmt, location)) +
		        list(struct.pack(self._uint32_fmt, length * 4)))

		data = self._fret.command(0xf0, args, expected_length = length * 4)

		return bytes(data)

	#def __setitem__(self, key, value):
		#start, length, single = _process_key(key, self.MAXMEM)
		#if single:
			#value = [value]
		#else:
			#value = list(value)

		#self.i2c.transfer(self.i2c_address, [I2C.Message([start, 0x00] + value)])

class Fret(object):
	def __init__(self, i2c, i2c_address = DEFAULT_I2C_ADDRESS):
		self.i2c = i2c
		self.i2c_address = i2c_address
		self.memory = FretMemory(self)

		self.set_demo_mode(False)

	def command(self, command_id, args=[], expected_length=100):
		if expected_length == 0:
			#it really doesn't like it yet
			expected_length = 1

		transmit = I2C.Message([command_id] + args)
		recieve = I2C.Message([0x00] * expected_length, read=True)

		self.i2c.transfer(self.i2c_address, [transmit, recieve])
		return recieve.data

	@property
	def chip_id(self):
		return self.command(0x00, expected_length=1)[0]

	@property
	def version(self):
		string = bytes(self.command(0x01))
		return string[:string.index(0)]

	@property
	def compile_time(self):
		string = bytes(self.command(0x02))
		return string[:string.index(0)]

	@property
	def touch(self):
		raws = self.command(0x10, expected_length=6*2)
		lsbs=raws[0::2]
		msbs=raws[1::2]

		return tuple(msb*256 + lsb for msb, lsb in zip(msbs, lsbs))

	def set_leds(self, data):
		return self.command(0x20, data, expected_length=0)

	def set_demo_mode(self, on = True):
		self.command(0xe0, [int(on)], expected_length=0)

def dynamic_led_test(fret):
	def rotate(l,n):
		return l[n:] + l[:n]

	colors = (
		[[255-x,0,x] for x in range(255)] +
		[[0,x,255-x] for x in range(255)] +
		[[x,255-x,0] for x in range(255)]
	)
	#colors=colors[::6] #faster
	#colors = [[b,b,b] for r,g,b in colors] #white glow
	#colors = [[r//5,g//5,b//5] for r,g,b in colors] #lower brightness

	print("Watch the pretty colors!")
	while True:
		for j in range(len(colors)):
			c=rotate(colors,j)[::len(colors)//20]
			#for g in range(20//10):
				##what if we had more frets(20x slower), was doing touch(2x slower), but increased the i2c speed(10x faster) too
			fret.set_leds(sum(c,[])[:18])
			#for g in range(20//10):
				##blink test to show the fps
				#r[0x80:]=[0x00]*6*3+[0x00]
			grayscale = " .:-=+*#%@"
			touch = fret.touch
			for t in touch:
				print ("%27s" % (grayscale[t % len(grayscale)] + grayscale[-1] * int(t / len(grayscale)),), end="")
			print()


def touch_test(fret):
	while True:
		raws=fret.registers[0x40:0x40+6*2]

		lsbs=raws[0::2]
		msbs=raws[1::2]

		print(("%10d" * 6) % tuple(msb*256 + lsb for msb, lsb in zip(msbs, lsbs)))

if __name__=="__main__":
	i2c = I2C("/dev/i2c-0")
	fret = Fret(i2c)
	dynamic_led_test(fret)
