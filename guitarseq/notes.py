#!/usr/bin/env python
import math
import collections

_octave="C C# D D# E F F# G G# A A# B".split()
notes = collections.OrderedDict()

class Note(object):
	def __new__(cls, note):
		if type(note) == int:
			self = object.__new__(Note)
			self.id = note
		elif type(note) == str:
			self = notes[note]
		elif type(note) == cls:
			return note
		else:
			raise ValueError("Invalid argument %r", id)
		return self
	
	def __repr__(self):
		return "Note('%s')" % (self)
	
	def __str__(self):
		return "%s%s" % (_octave[self.id % 12], self.id / 12 - 1)
	
	def __add__(self,other):
		try:
			otherid=other.id
		except:
			otherid=other
		return Note(self.id+otherid)
	
	def __cmp__(self,other):
		try:
			otherid=other.id
		except:
			otherid=other
		return cmp(self.id,otherid)
	
	def __sub__(self,other):
		try:
			otherid=other.id
		except:
			otherid=other
		return Note(self.id-otherid)
	
	@property
	def frequency(self):
		return math.pow(2,(self.id-69.0)/12)*440

for i in range(128):
	note = Note(i)
	notes[str(note)] = note

if __name__=="__main__":
	for note in notes.values():
		print("%d\t%r\t%f" % (note.id, note, note.frequency))
