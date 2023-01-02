use <stepper.scad>

$fn= 20;


THICK = 2;
M5=5.5;
M4=4.5;
M3=3.5;
M2=2.5;
M3INSERT=3.7;
M2INSERT=3.4;







rotate([0,180,0]) stepper_28byj48();

//table();

//testPointer();
//diskPointer();

//ledRing12();


//slotDetect();
//slotMount();



module table(){
    wid = 50;
    leng = 60;
    height = 25;
    ringMount=38;
    translate([0,0,THICK/2]){
        difference(){
            cube([wid, leng, THICK], center=true);
            cutout_28byj48();
           
            //Ring Mounts
            translate([0, 8, 0]){
                for (a=[0, 90, 180, 270]){
                    rotate([0,0,a])
                    translate([ringMount/2+M3/2, 0, 0]){
                        cylinder(d=M3-0.3, h=THICK*2, center=true);
                    }
                }
            }
            
            //General Mounts
            for (y=[-5,-15,-25,15, 25]){
                for (x=[0:10: (wid-M3)/2]){
                    translate([x, y, 0])
                        cylinder(d=M3,h=THICK*2, center = true);
                    translate([x * -1, y, 0])
                        cylinder(d=M3,h=THICK*2, center = true);
                }
            }
        }
        //LEGS
        for (y=[-1,1])
        translate([0, leng/2 * y, height/-2 + THICK/2]){
            difference(){
                cube([wid, THICK, height], center = true);
                for (z=[height/-2 + M3: 10: height/2 - M3]){
                    for (x=[0: 10: wid/2 - M3]){
                        for (i=[-1,1])
                            translate([ x * i, 0, z])
                                rotate([90,0,0])
                                    cylinder(d=M3, h= THICK*2, center=true);
                    }
                }
            }
        }
    }
}


module cutout_28byj48(){
    //Side M3 Bolts
    for (i=[-1, 1]){
        translate([17 * i,0,0]){
            cylinder(d=M3, h=THICK*3, center=true);
        }
    }
    
    translate([0,8,0]){
        cylinder(d=12, h=THICK*3, center=true);
    }
}


module hub_28byj48(){
    spindleD = 5.2;
    spindleX = 3.2;
    hubLen = 10;
    hubDiam = 14;
    translate([0, 8, THICK*1.5 + hubLen/2]){
        difference(){
            union(){
                cylinder(d=hubDiam, h=hubLen, center=true);
                cube([hubDiam/1.5, hubDiam , hubLen], center=true);
            }
            
            //Shaft
            intersection(){
                cylinder(d=spindleD, h=hubLen+THICK, center=true);
                cube([spindleX, hubDiam,hubLen+THICK], center=true);
            }
            
        
        
            // M2 Holes
            for (i=[-1,1]){
                translate([0, i*4, -3]){
                    rotate([0,90,0]) {
                        cylinder(d=M2, h= hubDiam *2, center=true);
                        translate([0,0,hubDiam/2])
                            cylinder(d=M2INSERT, 8, center=true);
                    }
                }
                translate([0, i*4, 1]){
                    rotate([0,90,0]){
                        cylinder(d=M2, h= hubDiam *2, center=true);
                        translate([0,0,hubDiam/2])
                            cylinder(d=M2INSERT, 8, center=true);
                    }
                }
            }
            
            
            //Slot
            translate([0,0,-2]){
                cube([1, hubDiam*1.5, hubLen], center=true);
            }
            
        }
        
    }
}






module testPointer(){
    pointerLen=20;
    hub_28byj48();
    translate([0,-3,10 + THICK]){
        cube([10, pointerLen, THICK], center=true);
        translate([0, pointerLen/-2, 0]){
            rotate([0,0,45])
                cube([10,10,THICK], center=true);
        }
    }
    
}


module diskPointer(){
    pointerLen=20;
    diam = 60;
    wid=10;
    hub_28byj48();
    spokeW=6;
    spokeL = diam -wid;
    slot = 2;
    
    
    translate([0, 8, 12]){
        difference(){
            cylinder(d=diam, h=THICK, center = true);
            cylinder(d=diam-(wid*2), h=2*THICK, center = true);
            translate([diam/2, 0, 0])
                cube([wid, slot, THICK*2], center=true);
        }
        
        
        cube([spokeW, spokeL, THICK ], center = true);
        difference(){
            cube([spokeL, spokeW, THICK ], center = true);
            translate([-16, 0, 0])
                cylinder(d=M3, h=2*THICK, center = true);
        }
        
    }
    
    
    
    /*
    translate([0,-3,10 + THICK]){
        cube([10, pointerLen, THICK], center=true);
        translate([0, pointerLen/-2, 0]){
            rotate([0,0,45])
                cube([10,10,THICK], center=true);
        }
    }
    */
    
    
}


module ledRing12(){
    diam = 38.5;
    wid = 6;
    height = 3.5;
    
    color("#771010")
    translate([0, 8, 4]){
        difference(){
            cylinder(d=diam, h=height, center = true);
            cylinder(d=diam-(wid *2), h=height*2, center = true);
        }
    }
    
}


module slotDetector(){
    leng=30;
    wid=14;
    dep=2;
    detD=12;
    detL=6;
    detW=12.8;
    slotW=4.9;
    slotD=8;
    hole=6;
    spacer=6;
    
    //pcb
    hx = ((leng/2)-hole) * (-1);
    difference(){
        cube([leng, wid, dep], center=true);
        translate([hx,0,0]){
            cylinder(d=M3, h= dep*4, center=true);
        }
    }
    
    //Spacer
    /*
    translate([hx,0,(spacer+dep)/(-2)-0.2]){
        cube([spacer,spacer,spacer], center=true);
    }
    */
    
    //slot
    x=(leng/2)-(detL/2);
    translate([x, 0, detD/2]){
        difference(){
            cube([detL, detW, detD], center=true);
            z=(slotD/2) - dep;
            translate([0,0,z+2]){
                cube([detL*2, slotW, slotD+4], center=true);
            }
        }
    }
}


module slotDetect(){
    //translate([-13,-20,4]) slotDetector();
    color("#00a020")
    translate([-13,-28, 12]){
        rotate([-90,0,0])
            slotDetector();
    }
}



module slotMount(){
    leng = 65 + THICK;
    height = 25;
    wid = 50;
    
    h2 = 20;
    
    
    translate([0, leng/-2 , height/-2 + THICK * 1 ]){
        difference(){
            cube([wid, THICK, height], center = true);
            for (z=[height/-2 + M3: 10: height/2 - M3]){
                for (x=[0: 10: wid/2 - M3]){
                    for (i=[-1,1])
                        translate([ x * i, 0, z])
                            rotate([90,0,0])
                                cylinder(d=M3, h= THICK*2, center=true);
                }
            }
        }
        
        translate([0,0, h2]){
            difference(){
                cube([wid, THICK, h2], center = true);
                for (x=[-5:1:5])
                translate([-18+x,0,2])
                    rotate([90, 0, 0])
                        cylinder(d=M3-0.5, h=THICK*2, center = true);
                cube([M2-0.2, THICK*2, h2/3*2], center=true);
            }
        }
    }
    
    
}