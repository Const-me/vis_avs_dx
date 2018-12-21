#pragma once

namespace ColorMod
{
	typedef struct
	{
		const char *name;
		const char *init;
		const char *point;
		const char *frame;
		const char *beat;
		int recompute;
	}
	presetType;

	static presetType presets[] =
	{
		// Name, Init, Level, Frame, Beat, Recalc
		{"4x Red Brightness, 2x Green, 1x Blue","","red=4*red; green=2*green;","","",0},
		{"Solarization","","red=(min(1,red*2)-red)*2;\r\ngreen=red; blue=red;","","",0},
		{"Double Solarization","","red=(min(1,red*2)-red)*2;\r\nred=(min(1,red*2)-red)*2;\r\ngreen=red; blue=red;","","",0},
		{"Inverse Solarization (Soft)","","red=abs(red - .5) * 1.5;\r\ngreen=red; blue=red;","","",0},
		{"Big Brightness on Beat","scale=1.0","red=red*scale;\r\ngreen=red; blue=red;","scale=0.07 + (scale*0.93)","scale=16",1},
		{"Big Brightness on Beat (Interpolative)","c = 200; f = 0;","red = red * t;\r\ngreen=red;blue=red;","f = f + 1;\r\nt = (1.025 - (f / c)) * 5;","c = f;f = 0;",1},
		{"Pulsing Brightness (Beat Interpolative)","c = 200; f = 0;","red = red * st;\r\ngreen=red;blue=red;","f = f + 1;\r\nt = (f * 2 * $PI) / c;\r\nst = sin(t) + 1;","c = f;f = 0;",1},
		{"Rolling Solarization (Beat Interpolative)","c = 200; f = 0;","red=(min(1,red*st)-red)*st;\r\nred=(min(1,red*2)-red)*2;\r\ngreen=red; blue=red;","f = f + 1;\r\nt = (f * 2 * $PI) / c;\r\nst = ( sin(t) * .75 ) + 2;","c = f;f = 0;",1},
		{"Rolling Tone (Beat Interpolative)","c = 200; f = 0;","red = red * st;\r\ngreen = green * ct;\r\nblue = (blue * 4 * ti) - red - green;","f = f + 1;\r\nt = (f * 2 * $PI) / c;\r\nti = (f / c);\r\nst = sin(t) + 1.5;\r\nct = cos(t) + 1.5;","c = f;f = 0;",1},
		{"Random Inverse Tone (Switch on Beat)","","dd = red * 1.5;\r\nred = pow(dd, dr);\r\ngreen = pow(dd, dg);\r\nblue = pow(dd, db);","","token = rand(99) % 3;\r\ndr = if (equal(token, 0), -1, 1);\r\ndg = if (equal(token, 1), -1, 1);\r\ndb = if (equal(token, 2), -1, 1);",1},
	};

	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addInput( "beat", eVarType::u32 );	// IS_BEAT
			addOutput( "red" );
			addOutput( "green" );
			addOutput( "blue" );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

namespace Move
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addInput( "b", eVarType::u32 );	// IS_BEAT
			addInput( "w", eVarType::u32 );	// screen width in pixels
			addInput( "h", eVarType::u32 );	// screen height in pixels

			addState( "alpha", 0.5f );

			addOutput( "d" );
			addOutput( "r" );
			addOutput( "x" );
			addOutput( "y" );
		}
	};

	typedef struct
	{
		const char *name;
		int rect;
		int wrap;
		int grid1;
		int grid2;
		const char *init;
		const char *point;
		const char *frame;
		const char *beat;
	} presetType;

	static presetType presets[] =
	{
	  {"Random Rotate", 0, 1, 2, 2, "","r = r + dr;","","dr = (rand(100) / 100) * $PI;\r\nd = d * .95;"},
	  {"Random Direction", 1, 1, 2, 2, "speed=.05;dr = (rand(200) / 100) * $PI;","x = x + dx;\r\ny = y + dy;","dx = cos(dr) * speed;\r\ndy = sin(dr) * speed;","dr = (rand(200) / 100) * $PI;"},
	  {"In and Out", 0, 1, 2, 2, "speed=.2;c=0;","d = d * dd;","","c = c + ($PI/2);\r\ndd = 1 - (sin(c) * speed);"},
	  {"Unspun Kaleida", 0, 1, 33, 33, "c=200;f=0;dt=0;dl=0;beatdiv=8","r=cos(r*dr);","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndr = 4+(cos(dt)*2);","c=f;f=0;dl=dt"},
	  {"Roiling Gridley", 1, 1, 32, 32, "c=200;f=0;dt=0;dl=0;beatdiv=8","x=x+(sin(y*dx) * .03);\r\ny=y-(cos(x*dy) * .03);","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndx = 14+(cos(dt)*8);\r\ndy = 10+(sin(dt*2)*4);","c=f;f=0;dl=dt"},
	  {"6-Way Outswirl", 0, 0, 32, 32, "c=200;f=0;dt=0;dl=0;beatdiv=8","d=d*(1+(cos(r*6) * .05));\r\nr=r-(sin(d*dr) * .05);\r\nd = d * .98;","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndr = 18+(cos(dt)*12);","c=f;f=0;dl=dt"},
	  {"Wavy", 1, 1, 6, 6, "c=200;f=0;dx=0;dl=0;beatdiv=16;speed=.05","y = y + ((sin((x+dx) * $PI))*speed);\r\nx = x + .025","f = f + 1;\r\nt = ( (f * 2 * 3.1415) / c ) / beatdiv;\r\ndx = dl + t;","c = f;\r\nf = 0;\r\ndl = dx;"},
	  {"Smooth Rotoblitter", 0, 1, 2, 2, "c=200;f=0;dt=0;dl=0;beatdiv=4;speed=.15","r = r + dr;\r\nd = d * dd;","f = f + 1;\r\nt = ((f * $pi * 2)/c)/beatdiv;\r\ndt = dl + t;\r\ndr = cos(dt)*speed*2;\r\ndd = 1 - (sin(dt)*speed);","c=f;f=0;dl=dt"},
	};
}

namespace SuperScope
{
	typedef struct
	{
		const char *name;
		const char *init;
		const char *point;
		const char *frame;
		const char *beat;
	} presetType;

	static presetType presets[] =
	{
	#ifdef LASER
	  {"Laser - Bouncing Line","n=8;xp=0;yp=0;d=0.5;r=0;xps=0; yps=0; dtr=0.1; bv=1; gv=1; rv=1;","x=xp+d*cos(r)*sign(i-0.5); y=yp+d*sin(r)*sign(i-0.5);","r=r+dtr; xp=xp*0.99+xps*0.01; yp=yp*0.99+yps*0.01; red=rv; green=gv; blue=bv;","xps=(rand(100)-50)/50; yps=(rand(100)-50)/50; dtr=-dtr; rv=rand(100)/100; gv=rand(100)/100; bv=rand(100)/100;"},
	  {"Laser - BeatFlex Scope","n=5; tv=0; dtv=0; tvs=1.0;","x=(i-0.5)*2; y=-sin(i*$PI)*tv+v*0.2;","tv=tv*0.97+tvs*0.03;","tvs=-sign(tvs);"},
	  {"Laser - Pulsing Box","n=5;r=$PI/4;","x=cos(i*$PI*2+r); y=sin(i*$PI*2+r); red=rv; green=gv; blue=bv;","rv=rv*0.93; gv=gv*0.93; bv=bv*0.93;","bv=blue; gv=green;rv=red;"},
	#else
	  {"Spiral","n=800","d=i+v*0.2; r=t+i*$PI*4; x=cos(r)*d; y=sin(r)*d","t=t-0.05",""},
	  {"3D Scope Dish", "n=200","iz=1.3+sin(r+i*$PI*2)*(v+0.5)*0.88; ix=cos(r+i*$PI*2)*(v+0.5)*.88; iy=-0.3+abs(cos(v*$PI)); x=ix/iz;y=iy/iz;","",""},
	  {"Rotating Bow Thing","n=80;t=0.0;","r=i*$PI*2; d=sin(r*3)+v*0.5; x=cos(t+r)*d; y=sin(t-r)*d","t=t+0.01",""},
	  {"Vertical Bouncing Scope","n=100; t=0; tv=0.1;dt=1;","x=t+v*pow(sin(i*$PI),2); y=i*2-1.0;","t=t*0.9+tv*0.1","tv=((rand(50.0)/50.0))*dt; dt=-dt;"},
	  {"Spiral Graph Fun","n=100;t=0;","r=i*$PI*128+t; x=cos(r/64)*0.7+sin(r)*0.3; y=sin(r/64)*0.7+cos(r)*0.3","t=t+0.01;","n=80+rand(120.0)"},
	  {"Alternating Diagonal Scope","n=64; t=1;","sc=0.4*sin(i*$PI); x=2*(i-0.5-v*sc)*t; y=2*(i-0.5+v*sc);","","t=-t;"},
	  {"Vibrating Worm","n=w; dt=0.01; t=0; sc=1;","x=cos(2*i+t)*0.9*(v*0.5+0.5); y=sin(i*2+t)*0.9*(v*0.5+0.5);","t=t+dt;dt=0.9*dt+0.001; t=if(above(t,$PI*2),t-$PI*2,t);","dt=sc;sc=-sc;"},
	  {"Wandering Simple","n=800;xa=-0.5;ya=0.0;xb=-0.0;yb=0.75;c=200;f=0;\r\nnxa=(rand(100)-50)*.02;nya=(rand(100)-50)*.02;\r\nnxb=(rand(100)-50)*.02;nyb=(rand(100)-50)*.02;","//primary render\r\nx=(ex*i)+xa;\r\ny=(ey*i)+ya;\r\n\r\n//volume offset\r\nx=x+ ( cos(r) * v * dv);\r\ny=y+ ( sin(r) * v * dv);\r\n\r\n//color values\r\nred=i;\r\ngreen=(1-i);\r\nblue=abs(v*6);","f=f+1;\r\nt=1-((cos((f*3.1415)/c)+1)*.5);\r\nxa=((nxa-lxa)*t)+lxa;\r\nya=((nya-lya)*t)+lya;\r\nxb=((nxb-lxb)*t)+lxb;\r\nyb=((nyb-lyb)*t)+lyb;\r\nex=(xb-xa);\r\ney=(yb-ya);\r\nd=sqrt(sqr(ex)+sqr(ey));\r\nr=atan(ey/ex)+(3.1415/2);\r\ndv=d*2","c=f;\r\nf=0;\r\nlxa=nxa;\r\nlya=nya;\r\nlxb=nxb;\r\nlyb=nyb;\r\nnxa=(rand(100)-50)*.02;\r\nnya=(rand(100)-50)*.02;\r\nnxb=(rand(100)-50)*.02;\r\nnyb=(rand(100)-50)*.02"},
	  {"Flitterbug","n=180;t=0.0;lx=0;ly=0;vx=rand(200)-100;vy=rand(200)-100;cf=.97;c=200;f=0","x=nx;y=ny;\r\nr=i*3.14159*2; d=(sin(r*5*(1-s))+i*0.5)*(.3-s);\r\ntx=(t*(1-(s*(i-.5))));\r\nx=x+cos(tx+r)*d; y=y+sin(t-y)*d;\r\nred=abs(x-nx)*5;\r\ngreen=abs(y-ny)*5;\r\nblue=1-s-red-green;","f=f+1;t=(f*2*3.1415)/c;\r\nvx=(vx-(lx*.1))*cf;\r\nvy=(vy-(ly*.1))*cf;\r\nlx=lx+vx;ly=ly+vy;\r\nnx=lx*.001;ny=ly*.001;\r\ns=abs(nx*ny)","c=f;f=0;\r\nvx=vx+rand(600)-300;vy=vy+rand(600)-300"},
	  {"Spirostar","n=20;t=0;f=0;c=200;mn=10;dv=2;dn=0","r=if(b,0,((i*dv)*3.14159*128)+(t/2));\r\nx=cos(r)*sz;\r\ny=sin(r)*sz;","f=f+1;t=(f*3.1415*2)/c;\r\nsz=abs(sin(t-3.1415));\r\ndv=if(below(n,12),(n/2)-1,\r\n    if(equal(12,n),3,\r\n    if(equal(14,n),6,\r\n    if(below(n,20),2,4))))","bb = bb + 1;\r\nbeatdiv = 8;\r\nc=if(equal(bb%beatdiv,0),f,c);\r\nf=if(equal(bb%beatdiv,0),0,f);\r\ng=if(equal(bb%beatdiv,0),g+1,g);\r\nn=if(equal(bb%beatdiv,0),(abs((g%17)-8) *2)+4,n);"},
	  {"Exploding Daisy","n = 380 + rand(200) ; k = 0.0; l = 0.0; m = ( rand( 10 ) + 2 ) * .5; c = 0; f = 0","r=(i*3.14159*2)+(a * 3.1415);\r\nd=sin(r*m)*.3;\r\nx=cos(k+r)*d*2;y=(  (sin(k-r)*d) + ( sin(l*(i-.5) ) ) ) * .7;\r\nred=abs(x);\r\ngreen=abs(y);\r\nblue=d","a = a + 0.002 ; k = k + 0.04 ; l = l + 0.03","bb = bb + 1;\r\nbeatdiv = 16;\r\nn=if(equal(bb%beatdiv,0),380 + rand(200),n);\r\nt=if(equal(bb%beatdiv,0),0.0,t);\r\na=if(equal(bb%beatdiv,0),0.0,a);\r\nk=if(equal(bb%beatdiv,0),0.0,k);\r\nl=if(equal(bb%beatdiv,0),0.0,l);\r\nm=if(equal(bb%beatdiv,0),(( rand( 100  ) + 2 ) * .1) + 2,m);"},
	  {"Swirlie Dots","n=45;t=rand(100);u=rand(100)","di = ( i - .5) * 2;\r\nx = di;sin(u*di) * .4;\r\ny = cos(u*di) * .6;\r\nx = x + ( cos(t) * .05 );\r\ny = y + ( sin(t) * .05 );","t = t + .15; u = u + .05","bb = bb + 1;\r\nbeatdiv = 16;\r\nn = if(equal(bb%beatdiv,0),30 + rand( 30 ),n);"},
	  {"Sweep","n=180;lsv=100;sv=200;ssv=200;c=200;f=0","sv=(sv*abs(cos(lsv)))+(lsv*abs(cos(sv)));\r\nfv=fv+(sin(sv)*dv);\r\nd=i; r=t+(fv * sin(t) * .3)*3.14159*4;\r\nx=cos(r)*d;\r\ny=sin(r)*d;\r\nred=i;\r\ngreen=abs(sin(r))-(red*.15);\r\nblue=fv","f=f+1;t=(f*2*3.1415)/c;\r\nlsv=slsv;sv=ssv;fv=0","bb = bb + 1;\r\nbeatdiv = 8;\r\nc=if(equal(bb%beatdiv,0),f,c);\r\nf=if(equal(bb%beatdiv,0),0,f);\r\ndv=if(equal(bb%beatdiv,0),((rand(100)*.01) * .1) + .02,dv);\r\nn=if(equal(bb%beatdiv,0),80+rand(100),n);\r\nssv=if(equal(bb%beatdiv,0),rand(200)+100,ssv);\r\nslsv=if(equal(bb%beatdiv,0),rand(200)+100,slsv);"},
	  {"Whiplash Spiral","n=80;c=200;f=0","d=i;\r\nr=t+i*3.14159*4;\r\nsdt=sin(dt+(i*3.1415*2));\r\ncdt=cos(dt+(i*3.1415*2));\r\nx=(cos(r)*d) + (sdt * .6 * sin(t) );\r\ny=(sin(r)*d) + ( cdt *.6 * sin(t) );\r\nblue=abs(x);\r\ngreen=abs(y);\r\nred=cos(dt*4)","t=t-0.05;f=f+1;dt=(f*2*3.1415)/c","bb = bb + 1;\r\nbeatdiv = 8;\r\nc=if(equal(bb%beatdiv,0),f,c);\r\nf=if(equal(bb%beatdiv,0),0,f);"},
	#endif
	};
}