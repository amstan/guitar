from periphery import *

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

class FretRegisters(object):
	COUNT = 256

	def __getitem__(self, key):
		start, length, single = _process_key(key, self.COUNT)

		msg = [
			I2C.Message([start, 0x00]),
			I2C.Message([0x00] * length, read=True)
		];

		self.i2c.transfer(self.i2c_address, msg);

		if single:
			return msg[1].data[0];
		else:
			return msg[1].data;

	def __setitem__(self, key, value):
		start, length, single = _process_key(key, self.COUNT)
		if single:
			value = [value]
		else:
			value = list(value)

		self.i2c.transfer(self.i2c_address, [I2C.Message([start, 0x00] + value)])

	def __repr__(self):
		return "FretRegisters"+repr(f[:])

class Fret(object):
	def __init__(self, i2c, address = DEFAULT_I2C_ADDRESS):
		self.registers = FretRegisters()
		self.registers.fret = self
		self.registers.i2c_address = address
		self.registers.i2c = i2c

	def chip_id(self):
		return self.registers[0]

def dynamic_led_test(fret):
	r=fret.registers

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
			c=rotate(colors,j)[::len(colors)//6]
			for g in range(20//10):
				##what if we had more frets(20x slower), was doing touch(2x slower), but increased the i2c speed(10x faster) too
				r[0x80:]=sum(c,[])+[0x00]
			#for g in range(20//10):
				##blink test to show the fps
				#r[0x80:]=[0x00]*6*3+[0x00]

def touch_test(fret):
	while True:
		fret.registers[0x80:]=[fret.registers[0x40],fret.registers[0x42],fret.registers[0x44]]*6+[0x00]

if __name__=="__main__":
	i2c = I2C("/dev/i2c-0")
	fret = Fret(i2c)
	dynamic_led_test(fret)
	#while True: print((("%02x "*3+"  ")*6)%tuple(fret.registers[0x80:0x80+6*3])) #can be used to see the colors that another python process is setting
