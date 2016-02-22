elecrow.zip:
	mkdir gerber.elecrow
	cp fret-B.Cu.gbr    gerber.elecrow/fret.GBL
	cp fret-B.Mask.gbr  gerber.elecrow/fret.GBS
	cp fret-B.SilkS.gbr gerber.elecrow/fret.GBO
	cp fret-F.Cu.gbr    gerber.elecrow/fret.GTL
	cp fret-F.Mask.gbr  gerber.elecrow/fret.GTS
	cp fret-F.SilkS.gbr gerber.elecrow/fret.GTO
	cp fret-Margin.gbr  gerber.elecrow/fret.GML
	cp fret.drl         gerber.elecrow/fret.TXT
	cd gerber.elecrow; zip ../$@ *
	rm -Rf gerber.elecrow

clean:
	-rm -Rf elecrow.zip
	-rm -Rf gerber.elecrow

clean-output:
	-rm -Rf *.gbr *.drl
