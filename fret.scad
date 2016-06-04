pcb_width = 65;
pcb_height = 25;
pcb_thick = 1.6;
plastic_min_width = 1;
curve_aspect_ratio = 0.6;

keepout_offset = 2.5;
keepout_height = 7;

pcb_color = [0.1, 0.1, 0.1];
plastic_color = [0, 0.7, 1];

module led() {
	color([1, 1, 1]) translate([0, 0, 0.5]) cube([5, 5, 1], center = true);
	color([0.9, 0.9, 0.9]) translate([0, 0, 0.6]) circle(2, $fn=100);
}

module touchpad() {
	color([0, 0, 0]) translate([]) cube([8, 15, 0.1], center = true);
}

module pcb() {
	translate([0, 0, pcb_thick/2])
		color(pcb_color) cube([pcb_width, pcb_height, pcb_thick], center = true);
//	translate([0, 0, -keepout_height/2])
//		color([1, 0.8, 0.5])
//			cube([pcb_width - keepout_offset * 2, pcb_height, keepout_height], center = true);
	for(i = [0 : 5]) {
		translate([6.25 - pcb_width / 2 + i * 10.5, 0, pcb_thick]) {
			translate([0, 21.25 - pcb_height / 2, 0]) {
				led();
			}
			translate([0, 9.75 - pcb_height / 2, 0]) {
				touchpad();
			}
		}
	}
}
pcb();

module lip() {
	polygon(points=[
		[pcb_width / 2, 0],
		[pcb_width / 2 + plastic_min_width, 0],
		[pcb_width / 2 + plastic_min_width, pcb_thick + plastic_min_width],
		[pcb_width / 2 - plastic_min_width, pcb_thick + plastic_min_width],
		[pcb_width / 2 - plastic_min_width, pcb_thick],
		[pcb_width / 2, pcb_thick],
	]);
//	polygon([[0,0],[100,0],[130,50],[30,50]]);
// polygon([[0,0],[100,0],[130,50],[30,50]],[[3,2,1,0]]);
// polygon([[0,0],[100,0],[130,50],[30,50]],[[1,0,3,2]]);

}

//color(plastic_color) rotate([90, 0, 0]) lip();

module plastic() {
	difference() {
		rotate([90, 0, 0]) {
			linear_extrude(height = pcb_height + 10, center = true) {
				union() {
					difference() {
						scale([1,curve_aspect_ratio])
							circle(pcb_width / 2 + plastic_min_width, $fn=500);
						translate([-pcb_width / 2 - plastic_min_width,0])
							scale(pcb_width + plastic_min_width*2) 
								square();
					}
					lip();
					mirror ([1,0,0]) lip();
				}
			}
		}
	translate([0, 0, -keepout_height/2])
		cube([pcb_width - keepout_offset * 2, 1000, keepout_height + 0.01], center = true);	
	}
}

color(plastic_color) plastic();