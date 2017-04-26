//Graph (for Arduboy)
//A way too ambitious project
//By V360

#include <Arduboy2.h>

Arduboy2 arduboy;

//Calculation
float func[3];
byte func_mode = 1;

//Graphing array
//There used to be three, but then I found out I could
//cut it down to two, then just one. RIP trace_x[] and fit[].
float raw[63];

//Window
//0 - xmin, 1 - xmax
//2 - ymin, 3 - ymax
float window[] = {-10,10,-10,10};

//Table
float table[5];
float table_rate = 1.0;
int table_ofs = -2;
boolean calcTable = true;

//Axis lines
float center_x;
float center_y;

//UI stuff
byte state = 0;
byte sel = 0;
float amount = 1.0;
boolean calcGraph = true;

void setup() {
  //Arduboy stuff
  arduboy.begin();

  //Set framerate to 30
  arduboy.setFrameRate(30);

  //Default function
  func[0] = 1;
}

void loop() {
  //Wait until next frame
  if (!(arduboy.nextFrame()))
    return;
  
  arduboy.pollButtons();
  arduboy.clear();
  
  if (calcGraph) {
    graph_calc();
    calcGraph = false;
  }
  
  if (arduboy.justPressed(A_BUTTON)) {
    state = 0;
    sel = 0;
    calcGraph = true;
    calcTable = true;
  }

  //Convserve code!
  if (state>2) {
    if (arduboy.justPressed(B_BUTTON)) { //efficient code.
      if (amount==0.1) {
        amount = 1.0;
      }
      else {
        amount = 0.1;
      }
    }
  }
  
  if (state==0) {
    //*********
    //*       *
    //* Graph *
    //*       *
    //*********
    if (arduboy.justPressed(  UP_BUTTON)&&sel>0)
      sel--;
    if (arduboy.justPressed(DOWN_BUTTON)&&sel<2)
      sel++;
    if (arduboy.justPressed(B_BUTTON)) {
      state = sel + 1;
      sel = 0;
    }
    
    //Graph
    graph_draw();
    
    //Menu background
    arduboy.fillRect(65,0,62,25,WHITE);

    //Text color
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE);
    
    //Menu items
    t(71, 0,"Function");
    t(71, 8,"Window");
    t(71,16,"Table");
    //Selection arrow
    arduboy.setCursor(65,sel*8);
    arduboy.write(26);

    //Text color
    arduboy.setTextColor(WHITE);
    arduboy.setTextBackground(BLACK);

    //Giant title
    arduboy.setTextSize(2);
    t(66,26,"Graph");
    //My name
    arduboy.setTextSize(1);
    t(66,43,"By: V360");
    t(66,54,"ver. 1.00");

    //Borders
    arduboy.drawFastVLine(64,0,64,WHITE);
    arduboy.drawFastVLine(127,0,64,WHITE);
    arduboy.drawFastHLine(65,41,36,WHITE);
    arduboy.drawFastHLine(105,41,22,WHITE);
    arduboy.drawFastHLine(65,52,62,WHITE);
    arduboy.drawFastHLine(65,63,62,WHITE);
  }
  else if (state==1) {
    //************
    //*          *
    //* Function *
    //*          *
    //************
    if (arduboy.justPressed(  UP_BUTTON)&&sel>0)
      sel--;
    if (arduboy.justPressed(DOWN_BUTTON)&&sel<7)
      sel++;
    if (arduboy.justPressed(B_BUTTON)) {
      func_mode = sel;
      state = 4;
      sel = 0;
    }
    
    //Menu background
    arduboy.fillRect(0,0,70,64,WHITE);
    
    //Text color
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE);
    
    //Menu items
    t(6, 0,"A");
    t(6, 8,"Ax+B");
    t(6,16,"Ax +Bx+C"); //^2
    t(6,24,"(x+A)(x+B)");
    t(6,32,"(A/x+B)+C");
    t(6,40," (x+A)+B"); //sqrt
    t(6,48,"|x+A|+B");
    t(6,56,"Ax^B+C");
    arduboy.setCursor(18,16);
    arduboy.write(253);
    arduboy.setCursor(6,40);
    arduboy.write(251);
    
    //Current choice dot
    arduboy.setCursor(0,func_mode*8);
    arduboy.write(7);

    //Selection arrow
    arduboy.setCursor(0,sel*8);
    arduboy.write(26);

    //Text color
    arduboy.setTextColor(WHITE);
    arduboy.setTextBackground(BLACK);
  }
  else if (state==2) {
    //**********
    //*        *
    //* Window *
    //*        *
    //**********
    if (arduboy.justPressed(   UP_BUTTON)&&sel>0)
      sel--;
    if (arduboy.justPressed( DOWN_BUTTON)&&sel<3)
      sel++;
    if (arduboy.justPressed( LEFT_BUTTON))
      window[sel]-=amount;
    if (arduboy.justPressed(RIGHT_BUTTON))
      window[sel]+=amount;
    
    //Menu background
    arduboy.fillRect(7,0,30,64,WHITE);

    //Text color
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE);

    //Title text
    t(7, 0,"X Min");
    t(7, 8,"X Max");
    t(7,16,"Y Min");
    t(7,24,"Y Max");
    //Amount
    t(7,56,"+");
    arduboy.setCursor(13,56);
    arduboy.print(amount);
    
    //Text color
    arduboy.setTextColor(WHITE);
    arduboy.setTextBackground(BLACK);

    //Selection arrow
    arduboy.setCursor(0,sel*8);
    arduboy.write(26);
    
    //Numbers
    for(byte i = 0; i < 4; i++){
      arduboy.setCursor(38,i*8);
      arduboy.print(window[i]);
    }

    //Borders
    arduboy.drawFastVLine(6,0,64,WHITE);
    arduboy.drawFastVLine(37,0,64,BLACK);
  }
  else if (state==3) {
    //*********
    //*       *
    //* Table *
    //*       *
    //*********
    if (arduboy.justPressed(   UP_BUTTON)&&table_ofs<999) {
      table_ofs-=table_rate;
      calcTable = true;
    }
    if (arduboy.justPressed( DOWN_BUTTON)&&table_ofs>-999) {
      table_ofs+=table_rate;
      calcTable = true;
    }
    if (arduboy.justPressed( LEFT_BUTTON)&&table_rate>-100) {
      table_rate-=amount;
      calcTable = true;
    }
    if (arduboy.justPressed(RIGHT_BUTTON)&&table_rate<100) {
      table_rate+=amount;
      calcTable = true;
    }

    //Calculate table
    if (calcTable) {
      if (table_rate<=0) {
        table_rate = amount;
      }
      for (byte i = 0; i < 5; i++) {
        table[i] = func_calc((i * table_rate) + table_ofs);
      }
      calcTable = false;
    }
    
    //Table
    for (byte i = 0; i < 5; i++) {
      arduboy.setCursor(0,i*8);
      arduboy.print((i * table_rate) + table_ofs);
      arduboy.setCursor(64,i*8);
      arduboy.print(table[i]);
    }
    
    //Other stuff
    t(0,48,"Rate:");
    arduboy.setCursor(36,48);
    arduboy.print(table_rate);
    t(0,56,"+");
    arduboy.setCursor(6,56);
    arduboy.print(amount);
    
    //Border
    arduboy.drawFastVLine(64,0,40,WHITE);
    arduboy.drawFastHLine(0,40,128,WHITE);
  }
  else if (state==4) {
    if (arduboy.justPressed(   UP_BUTTON)&&sel>0)
      sel--;
    if (arduboy.justPressed( DOWN_BUTTON)&&sel<2)
      sel++;
    if (arduboy.justPressed( LEFT_BUTTON))
      func[sel]-=amount;
    if (arduboy.justPressed(RIGHT_BUTTON))
      func[sel]+=amount;

    //Menu background
    arduboy.fillRect(7,0,30,64,WHITE);

    //Text color
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE);

    //Title text
    t(7, 0,"Var A");
    t(7, 8,"Var B");
    t(7,16,"Var C");
    //Amount
    t(7,56,"+");
    arduboy.setCursor(13,56);
    arduboy.print(amount);
    
    //Text color
    arduboy.setTextColor(WHITE);
    arduboy.setTextBackground(BLACK);

    //Selection arrow
    arduboy.setCursor(0,sel*8);
    arduboy.write(26);
    
    //Numbers
    for(byte i = 0; i < 3; i++){
      arduboy.setCursor(38,i*8);
      arduboy.print(func[i]);
    }

    //Borders
    arduboy.drawFastVLine(6,0,64,WHITE);
    arduboy.drawFastVLine(37,0,64,BLACK);
  }
  else {
    //*******
    //*     *
    //* BAD *
    //*     *
    //*******
    arduboy.setTextSize(2);
    t(0,0,"Oh no!");
    arduboy.setTextSize(1);
    arduboy.setCursor(0,16);
    arduboy.print("There's no state "+String(state)+"!");
  }

  arduboy.display();
}

//Graph stuff - calculate graph
void graph_calc() {
  center_x=(  ((-window[0])/(window[1]-window[0])))*63;
  center_y=(1-((-window[2])/(window[3]-window[2])))*63;

  for(float i = 0; i < 63; i++){
    raw[byte(floor(i))] = (((i+1)/63)*(window[1]-window[0]))+window[0];
  }

  for(byte i = 0; i < 63; i++){
    raw[i] = func_calc(raw[i]);
    raw[i] = (1-((raw[i]-window[2])/(window[3]-window[2])))*63;
  }
}

//Graph stuff - plot graph
void graph_draw() {
  arduboy.drawFastVLine(center_x,0,63,WHITE);
  arduboy.drawFastHLine(0,center_y,63,WHITE);
  
  for(byte i = 1; i < 63; i++){
    arduboy.drawLine(i-1,raw[i-1],i,raw[i],WHITE);
  }
}

float func_calc(float x) {
  if (func_mode==0) {
    return func[0];
  }
  else if (func_mode==1) {
    return (func[0]*x)+func[1];
  }
  else if (func_mode==2) {
    return (func[0]*pow(x,2))+(func[1]*x)+func[2];
  }
  else if (func_mode==3) {
    return (x+func[0])*(x+func[1]);
  }
  else if (func_mode==4) {
    return div_err(func[0],x+func[1])+func[2];
  }
  else if (func_mode==5) {
    return sqrt_err(x+func[0])+func[1];
  }
  else if (func_mode==6) {
    return abs(x+func[0])+func[1];
  }
  else if (func_mode==7) {
    return (func[0]*pow(x,func[1]))+func[2];
  }
  else {
    return 0;
  }
}

void t(byte x,byte y,char* str) {
  arduboy.setCursor(x,y);
  arduboy.print(str);
}

float div_err(float a,float b) {
  if (b==(float) 0) {
    return 0;
  }
  else {
    return a/b;
  }
}

float sqrt_err(float n) {
  if (n<(float) 0) {
    return 0;
  }
  else {
    return sqrt(n);
  }
}
