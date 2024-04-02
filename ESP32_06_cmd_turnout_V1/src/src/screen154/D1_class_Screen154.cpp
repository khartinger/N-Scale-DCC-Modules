//_____D1_class_Screen154.cpp____________________khartinger_____
// The class Screen154 extends the classes
//   U8G2_SSD1309_128X64_NONAME0_F_HW_I2C : U8G2
// with screen methods to write text on a OLED display.
// See also:
//   https://github.com/olikraus/u8g2
//   https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#ssd1309-128x64_noname0
//   https://github.com/olikraus/u8g2/wiki/fntlistallplain
//   https://github.com/olikraus/u8g2/wiki/u8g2reference
//
// Hardware: (1) WeMos D1 Mini or ESP32
//           (2) OLED Shield: 1.54" SSD1309, 128x64 pixel, I2C
// Created by Karl Hartinger, March 04, 2024
// Changes:
// 2024-03-04 New
// Released into the public domain.

#ifndef D1_CLASS_SCREEN154_H
#define D1_CLASS_SCREEN154_H
#include "src/screen154/D1_class_Screen154.h"

// *************************************************************
//       setter, getter, working methods
// *************************************************************

//_______set text start position in y direction_________________
// pos_: 1=Top, 2=Center, 3=Baseline, 4=Bottom
int Screen154::setFontPositionY(int pos_) {
 switch(pos_) {
  case FONT_TOP:
   BC::setFontPosTop(); fontPositionY=pos_; break;
  case FONT_CENTER:
   BC::setFontPosCenter(); fontPositionY=pos_; break;
  case FONT_BASELINE: 
   BC::setFontPosBaseline(); fontPositionY=pos_; break;
  case FONT_BOTTOM: 
   BC::setFontPosBottom(); fontPositionY=pos_; break;
  default: break;
 }
 return fontPositionY;
}

//_______set text start position in y direction_________________
// pos_: 1=Top, 2=Center, 3=Baseline, 4=Bottom
int Screen154::getFontPositionY() {
 return fontPositionY;
}

//_______set value of dotCounter for line ...___________________
// line_: 0=full screen counter, line from 1 to 8 (LINE_MAX_154)
// return: true=counter value set
bool Screen154::setDotCounter(int line_, int startpos_) {
 if(line_>=0 && line_<=LINE_MAX_154) {
  dotCounter[line_]=startpos_;
  return true;
 }
 return true;
}

//_______clear all dotCounter___________________________________
void Screen154::clearAllDotCounter() {
 for(int i=0; i<DOT_COUNTER_MAX; i++) 
  dotCounter[i]=0; // clear dot array
}


// *************************************************************
//       configure screens
// *************************************************************

//_____setup display and i2c____________________________________
void Screen154::begin() {
 _cp437=false;                         //
 clearAllDotCounter();                 // set all dot counter 0
 setFontTitle(FONT_TITLE);             // set font for title
 setFontText(FONT_TEXT);               // set font for text
 BC::setFont(FONT_TEXT);               // use default text font
 //BC::setFontRefHeightAll();            // maximum char height
 BC::setFontRefHeightText();           // Default: char height text
 //BC::setFontRefHeightExtendedText();
 setFontPositionY(3);                  // Default: baseline
 BC::begin();                          // reset,config,clear display
 BC::firstPage();                      // must be used with nextPage
 BC::clearBuffer();                    // clear display buffer
 BC::setFont(FONT_TEXT);               // set font
 Serial.println("Screen154::begin() finished!");
}

//_______show (only) first display______________________________
void Screen154::display() {
 //BC::display();
 do{ } while (BC::nextPage());
 BC::firstPage();
}

// *************************************************************
// two-color-screens with title and text
// *************************************************************

//_____display: title (16char) + 4 lines (21char), no border____
// align: l=left, c=center, r=right, L=left+overwrite, C, R...

void Screen154::screen14Clear(int line_, const String &text_)
{ screen14(line_, text_, 'l', true); }

void Screen154::screen14Clear(int line_, const String &text_, char align_)
{ screen14(line_, text_, align_, true); }

void Screen154::screen14(int line_, const String &text_)
{ screen14(line_, text_, 'l', false); }

void Screen154::screen14(int line_, const String &text_, char align_)
{ screen14(line_, text_, align_, false); }

void Screen154::screen14(int line_, const String &text_, 
  char align_, bool cls_)
{ screenXY(line_, text_, align_, cls_, 16, 12); }

void Screen154::screen14Dot(int line_)
{ 
  if(line_==1) screenXYDot(0,16,line_);
  else screenXYDot(16-12,12,line_); 
}

void Screen154::screen14Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen14Dot(line_);
  }
 }
 screen14Dot(line_);
}


//_____display: title (16char) + 5 lines (21char), no border____
// align: l=left, c=center, r=right, L=left+overwrite, C, R...
void Screen154::screen15Clear(int line_, const String &text_)
{ screen15(line_, text_, 'l', true); }

void Screen154::screen15Clear(int line_, const String &text_, char align_)
{ screen15(line_, text_, align_, true); }

void Screen154::screen15(int line_, const String &text_)
{ screen15(line_, text_, 'l', false); }

void Screen154::screen15(int line_, const String &text_, char align_)
{ screen15(line_, text_, align_, false); }

void Screen154::screen15(int line_, const String &text_, 
  char align_, bool cls_)
{ screenXY(line_, text_, align_, cls_, 18, 9); }

void Screen154::screen15Dot(int line_)
{ 
  if(line_==1) screenXYDot(0,16,line_);
  else screenXYDot(18-9,9,line_); 
}

void Screen154::screen15Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen15Dot(line_);
  }
 }
 screen15Dot(line_);
}

//_____display: title + 6 lines, no border______________________
void Screen154::screen16Clear(int line_, const String &text_)
{ screen16(line_, text_, 'l', true); }

void Screen154::screen16Clear(int line_, const String &text_, char align_)
{ screen16(line_, text_, align_, true); }

void Screen154::screen16(int line_, const String &text_)
{ screen16(line_, text_, 'l', false); }

void Screen154::screen16(int line_, const String &text_, char align_)
{ screen16(line_, text_, align_, false); }

void Screen154::screen16(int line_, const String &text_, 
  char align_, bool cls_)
{ screenXY(line_, text_, align_, cls_, 16, 8); }

void Screen154::screen16Dot(int line_)
{ 
  if(line_==1) screenXYDot(0,16,line_);
  else screenXYDot(16-8,8,line_); 
}

void Screen154::screen16Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen16Dot(line_);
  }
 }
 screen16Dot(line_);
}

// *************************************************************
// one-color-screen only with text (no title)
// *************************************************************

//_____display: 7 lines, no border______________________________
void Screen154::screen7Clear(int line_, const String &text_)
{ screen7(line_, text_, 'l', true); }

void Screen154::screen7Clear(int line_, const String &text_, char align_)
{ screen7(line_, text_, align_, true); }

void Screen154::screen7(int line_, const String &text_)
{ screen7(line_, text_, 'l', false); }

void Screen154::screen7(int line_, const String &text_, char align_)
{ screen7(line_, text_, align_, false); }

void Screen154::screen7(int line_, const String &text_, 
  char align_, bool cls_)
{ screenX(line_, text_, align_, cls_, 0, 9); }

void Screen154::screen7Dot(int line_)
{ screenXDot(0,9,line_); }

void Screen154::screen7Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen7Dot(line_);
  }
 }
 screen7Dot(line_);
}

//_____display: 8 lines, no border______________________________
void Screen154::screen8Clear(int line_, const String &text_)
{ screen8(line_, text_, 'l', true); }

void Screen154::screen8Clear(int line_, const String &text_, char align_)
{ screen8(line_, text_, align_, true); }

void Screen154::screen8(int line_, const String &text_)
{ screen8(line_, text_, 'l', false); }

void Screen154::screen8(int line_, const String &text_, char align_)
{ screen8(line_, text_, align_, false); }

void Screen154::screen8(int line_, const String &text_, 
  char align_, bool cls_)
{ screenX(line_, text_, align_, cls_, 0, 8); }

void Screen154::screen8Dot(int line_)
{ screenXDot(0,8,line_); }

void Screen154::screen8Dot(int line_, int initvalue)
{
 if(line_>0 && line_<DOT_COUNTER_MAX) {
  for(int i=0; i<initvalue; i++)
  {
   dotCounter[line_]=i;
   screen8Dot(line_);
  }
 }
 screen8Dot(line_);
}

//**************************************************************
// special screens
//**************************************************************

//_____clear display and draw a border__________________________
void Screen154::dotClear()
{
 dotClear("");
}

//_____clear display, write title and draw a border_____________
// dotCounter is set to 0, NO dot is plotted
void Screen154::dotClear(String title_)
{
 dotCounter[0]=0;                      // new start
 clear();                              // clear display + buffer
 //setDrawColor(BLACK);                  // background=black
 //fillRect(0,0,displayWidth,displayHeight); // clear all
 //setDrawColor(WHITE);                  // draw white
 drawRect(0,16,displayWidth,40);       // draw a rectangle
 fillRect(0,55,displayWidth,9);        // clear text area last line
 //------save old font position---------------------------------
 int iFontPos=getFontPositionY();      // remember font position
 setFontPositionY(1);                  // y font position 1=top
 useFontTitle();                       // set title font
 drawString(0,0,title_);               // write title
 useFontText();                        // set text font
 //------restore old font position------------------------------
 setFontPositionY(iFontPos);           // restore old fond pos
}

//_____draw a dot_______________________________________________
void Screen154::dot()
{
 if(dotCounter[0]>=80) 
 {
  dotCounter[0]=0;
  setDrawColor(BLACK);
  fillRect(0,16,displayWidth,40);      // clear dot field
  setDrawColor(WHITE);
  drawRect(0,16,displayWidth,40);      // draw a rectangle...
 }
 int xpos =  4 + 6*(dotCounter[0]%20); // cursor pos x
 int ypos = 20 + 8*int(dotCounter[0]/20); // cursor pos y
 drawString(xpos,ypos,String(DOT),FONT_TOP); // draw dot
 dotCounter[0]++;                      // next vlue
 sendBuffer();                         // display buffer
}

//_____dot screen: write line 6_________________________________
void Screen154::dotLine(String line6)
{
 setDrawColor(WHITE);
 fillRect(0,55,displayWidth,displayHeight); // clear text area
 setDrawColor(BLACK);
 drawString(0,55,line6, FONT_TOP);     // draw dot
 setDrawColor(WHITE);
 sendBuffer();                         // display buffer
}

//_____dot screen: write title and plot a dot___________________
void Screen154::dotTitle(String title_)
{
 dotTitleOnly(title_);                 // write title
 dot();                                // draw a dot
}

//_____dot screen: write title__________________________________
void Screen154::dotTitleOnly(String title_)
{
 setDrawColor(BLACK);                  // background=black
 fillRect(0,0,displayWidth,16);        // clear title area
 setDrawColor(WHITE);                  // draw white
 setFont(fontTitle);                   // set title font
 drawString(0,0,title_,FONT_TOP);      // write title
 setFont(fontText);                    // set text font
 sendBuffer();                         // display buffer
}

//**************************************************************
// helper functions
//**************************************************************

//_____write lines (21char) to buffer (no special title)________
// Method does not display the line(s) --> extra call of display
// line_ : 1...8 (assume SSD1609 with 128x64 pixel)
// align_: l=left, c=center, r=right, L=left+overwrite, C, R...
// cls_  : true=clear display
// y0_   : y start of screen in pixels (usually 0)
// dline_: line spacing in pixels (8, 9, ...)
void Screen154::screenX(int line_, const String &text_, 
 char align_, bool cls_, int y0_, int dline_)
{
 int dx=0;                             // left space
 int dx2=0;                            // half char width
 int dy1=0;                            // start pixel of line
 int maxc_;                            // max. chars in one line
 int colorf=WHITE;                     // front color
 int colorb=BLACK;                     // background color
 String s_=text_;                      // remaining chars of text
 String s1;                            // text in this line
 int lenc_=utf8length(s_);             // string length in chars
 bool clr_=isLowerCase(align_);        // clear area before writing
 if(line_>LINE_MAX_154) return;        // line number ok?
 align_=tolower(align_);               // c,r,l
 //------clear display------------------------------------------
 if(cls_) {                            // do clear screen
  BC::clear();                         // clears display + buffer
  for(int i=0; i<DOT_COUNTER_MAX; i++) dotCounter[i]=0;
 }
 //------if the line number is negative, invert text------------
 if(line_<0) {
  colorf=BLACK;
  colorb=WHITE;
  line_=-line_;
 }
 //------prepare text output------------------------------------
 BC::setFont(fontText);                // set text font
 dx2=getFontWidth()/2;                 // half char
 maxc_=int(displayWidth/getFontWidth()); // max. chars in one line
 BC::setDrawColor(colorf);
 //-----write text----------------------------------------------
 for(int l1=line_; l1<9; l1++)
 {
  dy1=y0_+(l1-1)*dline_;
  if(dy1>0) dy1--; // Why necessary ???
  if(dy1+dline_> displayHeight) break;
  if(align_=='c'){ if((lenc_<maxc_) &&((lenc_&1)>0)) dx=dx2; }
  if(clr_) { 
   BC::setDrawColor(colorb);                // background color
   //BC::drawBox(0,dy1,displayWidth,dline_);  // clear line
   BC::drawBox(0,dy1+1,displayWidth,dline_);  // clear line
  }
  BC::setDrawColor(colorf);
  s1=mytrim(maxc_, s_, align_);
if(dy1>53)Serial.println("dy1="+String(dy1));
  drawString(dx,dy1,s1,FONT_TOP);
  if(lenc_<=maxc_) break;
  s_=utf8substring(s_,maxc_,0); 
  lenc_=utf8length(s_);
 }
 BC::setDrawColor(WHITE);              // Draw next one in white
}

//_____write lines (21char) to buffer (line 0|1 = title)________
// Method does not display the line(s) --> extra call of display
// line_ : display line 1 to lINE_MAX_154
//         line number <0: invert line
// align : l=left, c=center, r=right, L=left+overwrite, C, R...
// y0_   : y start position in pixels
// dline_: line spacing in pixels
void Screen154::screenXY(int line_, const String &text_, 
 char align_, bool cls_, int y0_, int dline_)
{
 int dx=0;                             // left space
 int dx2=0;                            // half char width
 int maxc_;                            // max. chars in one line
 int colorf=WHITE;                     // front color
 int colorb=BLACK;                     // background color
 String s_=text_;                      // remaining chars of text
 String s1;                            // text in this line
 int lenc_=utf8length(s_);             //  string length in chars
 bool clr_=isLowerCase(align_);        // clear area before writing
 if(line_>LINE_MAX_154) return;        // line number ok?
 align_=tolower(align_);               // c,r,l
 //------clear display------------------------------------------
 if(cls_) {                            // do clear screen
  BC::clear();                         // clear display + buffer
  for(int i=0; i<DOT_COUNTER_MAX; i++) dotCounter[i]=0;
 }
 if(line_==1 || line_==0 || line_==-1)
 {//-----write title--------------------------------------------
  if(line_==-1)
  {
   colorf=BLACK;
   colorb=WHITE;
  }
  BC::setFont(fontTitle);              // set title font
  dx2=getFontWidth()/2;                // half char
  maxc_=int(displayWidth/getFontWidth()); // max. chars in one line
  if(align_=='c'){ if((lenc_<maxc_) &&((lenc_&1)>0)) dx=dx2; }
  if(clr_) { 
   BC::setDrawColor(colorb);
   if(line_==0) BC::drawBox(1,1,getWidth()-2,2*dline_-2); //clear line 1
   BC::drawBox(0,0,getWidth(),2*dline_);     //clear line 1
  }
  BC::setDrawColor(colorf);
  s1=mytrim(maxc_, s_, align_);
  drawString(dx,0,s1,FONT_TOP);
  if(line_==0) BC::drawFrame(0,0,displayWidth,16); // border line 1
  //.....draw lines, show display...............................
  //BC::display();                       // show buffer
  BC::setDrawColor(WHITE);             // Draw next one in white
 }
 else
 {//-----write text---------------------------------------------
  if(line_>0) line_--; else line_++; 
  screenX(line_, text_, align_, cls_, y0_, dline_);
 }
}

//_____plot dots in a text line_________________________________
// y0_   : y start position in pixels
// dline_: line spacing in pixels
// line_ : display line 1 to lINE_MAX_154
void Screen154::screenXDot(int y0_, int dline_, int line_)
{
 int dx=0;                             // left space
 int dx1;                              // width of 1 char
 int xpos;                             // DOT x-position
 int ypos;                             // DOT y-position
 int maxc_;                            // max. chars in one line
 if((line_<1)||(line_>DOT_COUNTER_MAX)) return;
 //------save old font position---------------------------------
 int iFontPos=getFontPositionY();      // remember font position
 setFontPositionY(1);                  // y font position 1=top
 BC::setFont(fontText);                // set text font
 dx1=getFontWidth();                   // width of 1 char
 maxc_=int(displayWidth/dx1);          // max. chars in one line
 if((dotCounter[line_]>=maxc_)||(dotCounter[line_]<1))
  dotCounter[line_]=0;
 ypos=y0_+dline_*(line_-1);            // y position of dot
 xpos=dx+dx1*dotCounter[line_];        // x position of dot
 if(dotCounter[line_]==0)
 {
  BC::setDrawColor(BLACK);
  BC::drawBox(0,ypos,getWidth(),dline_); // clear line
  BC::setDrawColor(WHITE);
 }
 BC::drawGlyph(xpos,ypos,DOT);        // draw dot
 //------prepare next output------------------------------------
 sendBuffer();                         // display buffer
 dotCounter[line_]++;                  // next value
 setFontPositionY(iFontPos);           // restore old fond pos
}

//_____plot dots in a title or text line________________________
void Screen154::screenXYDot(int y0_, int dline_, int line_)
{
 int dx=0;                             // left space
 int dx1;                              // width of 1 char
 int xpos=0;                           // DOT x-position
 int ypos=y0_;                         // DOT y-position
 int maxc_;                            // max. chars in one line
 if((line_<1)||(line_>DOT_COUNTER_MAX)) return;
 //------save old font position---------------------------------
 int iFontPos=getFontPositionY();      // remember font position
 setFontPositionY(1);                  // y font position 1=top
 if(line_==1)
 {//-----write title--------------------------------------------
  BC::setFont(fontTitle);              // set title font
  dx1=getFontWidth();                  // width of 1 char
  xpos = dx + dx1*dotCounter[1];       // cursor pos x
  ypos = y0_;                          // cursor pos y
  maxc_=int(displayWidth/dx1);         // max. chars in one line
  if(dotCounter[line_]>=maxc_ ||dotCounter[line_]<1)
    dotCounter[line_]=0;
 if(dotCounter[line_]==0)
  {
   dotCounter[1]=0;
   BC::setDrawColor(BLACK);
   BC::drawBox(0,ypos,getWidth(),16);// clear line
   BC::setDrawColor(WHITE);
   BC::drawFrame(0,0,getWidth(),16); // draw a rectangle
  }
  BC::drawGlyph(xpos,ypos,DOT);        // draw dot
  dotCounter[1]++;                    // next value
  //-----show display line 1------------------------------------
  sendBuffer();
 }
 else
 {//-----write line >1------------------------------------------
  screenXDot(y0_, dline_, line_);
 }
 //------prepare next output------------------------------------
 setFontPositionY(iFontPos);           // restore old fond pos
}

//_____make string with max chars (add blank before/after)______
String Screen154::mytrim(int max_, String txt_, char align_='l')
{
 String s=txt_;
 //-----add leading blanks, if align_ needs them----------------
 int blank_=0;
 int len_;
 if(_cp437) len_=txt_.length();
 else len_=utf8length(txt_);
 if(align_=='c') blank_=(max_-len_)/2;
 if(align_=='r') { if(len_<max_) blank_=max_-len_; }
 //-----add blanks before text----------------------------------
 for(int i=0; i<blank_; i++) s=" "+s;
 if(len_>max_) 
 { //-----text too long, truncate text--------------------------
  s=utf8substring(s,0,max_);
 }
 else //-----text too short, add blank--------------------------
 {
  for(int i=len_; i<max_; i++) s+=" ";
 }
 return s;
}

// *************************************************************
//      font specific methods
// *************************************************************

 //_____get width of text (-1 on error)_________________________
 int Screen154::getFontWidth() {
  return(BC::getMaxCharWidth());
}

 //_____get height of text (-1 on error)________________________
 int Screen154::getFontHeight() {
  return BC::getMaxCharHeight();
}

//_____set font for title when using methods screen...___________
void Screen154::setFontTitle(const uint8_t *pfont1) {
  fontTitle=(uint8_t *)pfont1;
}

//_____set font for text when using methods screen...___________
void Screen154::setFontText(const uint8_t *pfont1) 
{ 
 fontText=(uint8_t *)pfont1;
}

//_______set font to title font for next drawing(s)_____________
void Screen154::useFontTitle() {
 BC::setFont(fontTitle);                    // activite font
}

//_______set font to text font for next drawing(s)______________
void Screen154::useFontText() {
 BC::setFont(fontText);                     // activite font
}

// *************************************************************
//      draw methods
// *************************************************************

//_____clear screen buffer______________________________________
void Screen154::clear()
{
 BC::clearDisplay();
}

//_____draw character array_____________________________________
void Screen154::drawChars(int16_t xMove, int16_t yMove, char *text, int16_t length)
{
 BC::drawStr(xMove, yMove, text);
}
/*
 uint16_t lineHeight = pgm_read_byte(fontData + HEIGHT_POS);
 char text_[length+1];
 strncpy(text_, text, length);
 text_[length]=0;  //..ToDo..
 uint16_t yOffset = 0;
 // If the string should be centered vertically too
 // we need to now how high the string is.
 if (textAlignment == TEXT_ALIGN_CENTER_BOTH) {
   uint16_t lb = 0;
   // Find number of linebreaks in text
   for (uint16_t i=0;text_[i] != 0; i++) {
     lb += (text_[i] == 10);
   }
   // Calculate center
   yOffset = (lb * lineHeight) >> 1;
 }
 uint16_t line = 0;
 char* textPart = strtok(text_,"\n");
 while (textPart != NULL) {
  uint16_t length = strlen(textPart);
  drawStringInternal(xMove, yMove - yOffset + (line++) * lineHeight, textPart, length, getStringWidth(textPart, length));
  textPart = strtok(NULL, "\n");
 }
}
*/

//_____write a string___________________________________________
void Screen154::drawString(int16_t x, int16_t y, String strUser) {
 BC::drawStr(x,y,strUser.c_str());
}

//_____write a string using given font position_________________
void Screen154::drawString(int16_t x, int16_t y, String strUser,
     int fontPosition)
{
 int iFontPos=getFontPositionY();      // remember font position
 setFontPositionY(fontPosition);       // y font position 1=top
 BC::drawStr(x,y,strUser.c_str());     // write string
 setFontPositionY(iFontPos);           // restore old fond pos
}

void Screen154::drawRect(int16_t x, int16_t y, int16_t width, int16_t height) {
 BC::drawFrame(x,y,width, height);
}

void Screen154::fillRect(int16_t x, int16_t y, int16_t width, int16_t height) {
 BC::drawBox(x,y,width, height);
}


//______draw a string (using chars from code page 437)__________
void Screen154::drawStringUsingCP437(int16_t xMove, int16_t yMove, String strUser) {
 int len_=utf8length(strUser);
 char ac[len_+1];
 stringToCP437(strUser,ac);
 ac[len_]=0;
 drawChars(xMove, yMove, ac, len_);
}

/*
//______draw a string (using chars from code page xxx)__________
void Screen154::drawStringUsingCPxxx(int16_t xMove,int16_t yMove,String strUser,int cp) {
 u8g2_.drawStr(xMove, yMove,strUser.c_str());
}

 uint16_t lineHeight = pgm_read_byte(fontData + HEIGHT_POS);
 char text[1+strUser.length()];
 char* ptext=text;
 int len1=0;
 switch(cp)
 {
  case 437: len1=stringToCP437(strUser, ptext); break;
  //case 850: len1=string2cp850(strUser, ptext); break;
  default: return;
 }
 text[len1]=0;
 uint16_t yOffset = 0;
 if (textAlignment == TEXT_ALIGN_CENTER_BOTH) {
  uint16_t lb = 0;
  // Find number of linebreaks in text
  for (uint16_t i=0;text[i] != 0; i++) {
    lb += (text[i] == 10);
  }
  // Calculate center
  yOffset = (lb * lineHeight) >> 1;
 }
 uint16_t line = 0;
 char* textPart = strtok(text,"\n");
 while (textPart != NULL) {
  len1 = strlen(textPart);
  drawStringInternal(xMove, yMove - yOffset + (line++) * lineHeight, textPart, len1, getStringWidth(textPart, len1));
  textPart = strtok(NULL, "\n");
 }
}
*/

// *************************************************************
//      utf8 specific methods
// *************************************************************

//_____number of characters in a UTF-8-string___________________
int Screen154::utf8length(String s1)
{
 //return BC::getUTF8Width(s1.c_str());
 int len=s1.length();                  // str length in bytes(!)
 int chars=0;                          // number of characters
 unsigned char byt;                    // help byte
 for(int i=0; i<len; i++)              // for all bytes in string
 {
  byt=s1.charAt(i);                    // 1 byte of given string
  chars++;                             // one more new character
  //.....is the byte a following utf8-byte?.....................
  if(((byt&0x80)==0x80) && ((byt|0xBF)==0xBF)) 
   chars--;                            // yes: do new character
 }
 return chars;                         // # of chars (NOT bytes)
}

//_____substring with len2 chars starting at from1______________
// index from1 starts with 0, len2<=0: until end of string
String Screen154::utf8substring(String source, int from1, int len2)
{
 int lenbytes=source.length();         // string length in bytes
 int chars=0;                          // number of utf8 chars
 int chars2=0;                         // utf8 chars in result
 unsigned char c1;                     // byte in string
 bool bcopy=false;                     // copy part of string
 String s2((char *)0);                 // result string
 s2.reserve(lenbytes);                 // maximum result length
 s2="";                                // start result
 //-----for all bytes in source string--------------------------
 for(int i1=0; i1<lenbytes; i1++)
 {
  if(chars==from1) bcopy=true;         // number of start char
  c1=source.charAt(i1);                // get byte of source
  if(bcopy)                            // if copy...
  { s2+=String((char)c1); chars2++; }  // add byte to result
  chars++;                             // next char in source
  c1=source.charAt(++i1);              // next byte in source
  //.....consists utf8-character of more than one byte?.........
  while( ((c1&0x80)==0x80) && ((c1|0xBF)==0xBF) ) {
   if(bcopy) s2+=String((char)c1);     // add byte to resulr
   c1=source.charAt(++i1);             // next byte of this char
  }
  --i1;                                // prepare next byte
  //.....number of chars to copy reached?....................... 
  if(len2>0) { if(chars2>=len2) return s2; }
 }
 return s2;                            // return result string
}

//_____number of bytes of utf8-char_____________________________
// return 1..4, 0 on error
int Screen154::utf8NumberOfBytes(uchar utf8[])
{
 uchar c0=utf8[0];
 if(c0==0) return 1;                             // 1x null byte
 if(c0<=0x7F) return 1;                          // 1 byte value
 if((c0&0xF0)==0xF0)
 {
  if((c0|0xF7)==0xF7) return 4;                  // 4 byte value
  else return 0;                                 //11111 = error
 }
 if(((c0&0xE0)==0xE0)&&((c0|0xEF)==0xEF)) return 3;// 3 byte value
 else
 {
  if(((c0&0xC0)==0xC0)&&((c0|0xDF)==0xDF)) return 2;
 }
 return 0;
}

//_____is given byte a first of n bytes_________________________
// return: 1..4=first of .. bytes or 0 if following byte (10xx)
// 0xxxxxxx                             00 to 7F, 1 byte
// 110xxxxx 10xxxxxx                    CX or DX, 2 byte
// 1110xxxx 10xxxxxx 10xxxxxx           EX      , 3 byte
// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  F0 to F7, 4 byte
int Screen154::utf8FirstOfNBytes(uchar c0)
{
 if((c0|0x7F)==0x7F) 
 {
  //if(c0==0x7F) return 3;                    // special char
  return 1;                                 // 1 byte value
 }
 if((c0&0xF0)==0xF0)
 {
  if((c0|0xF7)==0xF7) return 4;             // 4 byte value
  else return 0;                            // 11111xxx = error
 }
 if(((c0&0xE0)==0xE0)&&((c0|0xEF)==0xEF)) return 3;// 3 byte value
 else
 {
  if(((c0&0xC0)==0xC0)&&((c0|0xDF)==0xDF)) return 2;
 }
 return 0;
}

// *************************************************************
//     utf8 <-> cp437 methods
// *************************************************************

//_____convert a hole utf8-string to cp437 array________________
// ptext should point to char array with length 1+strUser.length()
// return: number of cp437 characters
// Called by drawString1()
// UTF-8:
// 0xxxxxxx                             00 to 7F, 1 byte
// 110xxxxx 10xxxxxx                    CX or DX, 2 byte
// 1110xxxx 10xxxxxx 10xxxxxx           EX      , 3 byte
// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  F0 to F7, 4 byte
int Screen154::stringToCP437(String strUser, char* ptext)
{
 int  len437=0;
 int  lenutf8=strUser.length();
 uchar ca1[1+lenutf8];
 for(int i=0; i<lenutf8;i++) ca1[i]=(uchar)strUser.charAt(i);
 ca1[lenutf8]=0;
 len437=aUtf8ToCP437(lenutf8, ca1, (uchar*) ptext);
 return len437;
}

//_____convert utf8 character to code page 437 char (= 1 byte)__
// return: cp850 character (0 on error)
uchar Screen154::utf8ToCP437(uchar utf8[])
{
 int   bytes=0;                   //number of bytes of utf8 char
 uchar c0=utf8[0];                // 1st byte
 bytes=utf8NumberOfBytes(utf8);
if(c0==0x7F) { Serial.print("bytes=");Serial.print(bytes); }
 if(bytes<1) return 0;                      // no valid value
 if(bytes>3) return 0;                      // no cp850 char
 if(bytes==1) return c0;                    // 1 byte value
 //-----search 2 or 3 byte value--------------------------------
 for(int i=0; i<128; i++)
 {
  if(pgm_read_byte(table_cp437_utf8+4*i+1)==c0)
  {
   if(pgm_read_byte(table_cp437_utf8+4*i+2)==utf8[1])
   {
    if(bytes==2) return pgm_read_byte(table_cp437_utf8+4*i);
    if(pgm_read_byte(table_cp437_utf8+4*i+3)==utf8[2])
    {
     if(bytes==3) return pgm_read_byte(table_cp437_utf8+4*i);
    }
   }
  }
 }
 return 0;
}

//_____convert cp437 char to utf8 char (1..3 bytes)_____________
// return: number of bytes of utf8 char (1..3), 0 on error
int Screen154::CP437ToUtf8(uchar cp437, uchar utf8[4])
{
 int i;
 if(cp437<=0x7F)
 {//-----one byte character-------------------------------------
  utf8[0]=cp437;
  utf8[1]=0;
  utf8[2]=0;
  utf8[3]=0;
  return 1;
 }
 //-----more than 1 byte for 1 utf8 character-------------------
 for(i=0; i<128; i++)                  // 80..FF=128 chars
 {
  if(cp437==pgm_read_byte(table_cp437_utf8+4*i))
  {
   utf8[0]=pgm_read_byte(table_cp437_utf8+4*i+1);
   utf8[1]=pgm_read_byte(table_cp437_utf8+4*i+2);
   utf8[2]=pgm_read_byte(table_cp437_utf8+4*i+3);
   utf8[3]=0;
   if(utf8[2]==0) return 2;
   return 3;
  }
 }
 return 0;
}

//_____convert cp437 char to String_____________________________
// return: (utf8-)string for given char
String Screen154::CP437ToString(uchar cp437)
{
 uchar utf8[4];
 int imax=CP437ToUtf8(cp437, utf8);
 String s1((char *)0);
 s1.reserve(imax+1);
 for(int i=0; i<imax; i++)
 {
  s1+=String((char)utf8[i]);
 }
 return s1;
}

//_____convert utf8 array to code page 437 array________________
// return: number of bytes of cp437 array
int Screen154::aUtf8ToCP437(int num, uchar autf8[], uchar acp437[])
{
 int i=0;
 int num437=0;
 int numutf8;
 uchar c0;
 uchar utf8[4];
 for(i=0; i<num; i++)
 {
  c0=autf8[i];
  if(c0==0) return num437;
  numutf8=utf8FirstOfNBytes(c0);
  if(i+numutf8>num) return num437;
  //-----prepare conversion-------------------------------------
  utf8[0]=c0;
  if(numutf8>1) utf8[1]=autf8[++i]; else utf8[1]=0;
  if(numutf8>2) utf8[2]=autf8[++i]; else utf8[2]=0;
  if(numutf8>3) utf8[3]=autf8[++i]; else utf8[3]=0;
  acp437[num437++]=utf8ToCP437(utf8);
 }
 return num437;
}

//_____convert cp850 array to utf8 array________________________
// return: number of bytes of utf8 array
int Screen154::aCP437ToUtf8(int num, uchar acp437[], uchar autf8[])
{
 int numutf8=0;
 int num1=0;
 uchar c0;
 uchar utf8[4];
 int i;
 //-----for all cp850 characters--------------------------------
 for(i=0; i<num; i++)
 {
  c0=acp437[i];
  if(c0==0) return numutf8;
  num1=CP437ToUtf8(acp437[i],utf8);
  if(num1==0) return numutf8;
  autf8[numutf8++]=utf8[0];
  if(num1>1) autf8[numutf8++]=utf8[1];
  if(num1>2) autf8[numutf8++]=utf8[2];
  if(num1>3) autf8[numutf8++]=utf8[3];
 }
 return numutf8;
}

//==================================================================================
/*
  Fontname: font_KH_6x8_mf
  Copyright: Public domain
  Glyphs: 224/224
  BBX Build Mode: 0
*/
const uint8_t u8g2_font_KH_cp437_6x8_mf[2279] U8G2_FONT_SECTION("u8g2_font_KH_cp437_6x8_mf") = 
  "\340\0\3\2\3\4\4\4\4\6\10\0\0\10\0\10\1\1:\2~\10\312 \5\0D\7!\7\271M"
  "\307\240\4\42\7\33mGb\11#\16\275LO)I\6\245\62(\245$\1$\14\275LW\66(\331"
  "\226\14Z\4%\12\275L\207&e\235\64\1&\15\275LO\226DIVI\244H\11'\7#e\217"
  "\245\4(\10;MWR\252\25)\11;MGV\252\224\0*\13\275LW\245rP\226\246\10+\12"
  "\255TW\30\15R\30\1,\7#E\217\245\4-\6\215d\307 .\6\222M\307\20/\7\255Tg"
  "\326\21\60\14\275L\317\222IK\62i\311\2\61\10;MO\42u\31\62\13\275L\317\20V\226\60\34"
  "\4\63\14\275L\307 f\231\252%\13\0\64\14\275L_&%\245d\320\302\4\65\13\275L\307qH"
  "C-Y\0\66\14\275L\327\222\205C\222i\311\2\67\11\275L\307 \326:\2\70\14\275L\317\222i"
  "\311\222i\311\2\71\14\275L\317\222i\311\20f\311\4:\6\231]G\22;\11\62EO\226(\11\0"
  "<\10<M_\324\330\0=\10\235\134\307\240\16\2>\10<MG\330\324\6\77\14\275L\317\222\205\221"
  "\226C\21\0@\15\275L\317\222)\211\62$J:\4A\14\275LW\226\324\264a\310\264\0B\15\275"
  "L\307\220d\332\240d\332\240\0C\12\275L\317\222\211m\311\2D\12\275L\307\220d\336\6\5E\13"
  "\275L\307\61\34\222\60\34\4F\12\275L\307\61\34\222\260\10G\13\275L\317\240\211\245-\31\2H\12"
  "\275LGf\33\206\314\26I\10;M\307\22u\31J\12\275L\327\26\266D\221\4K\15\275LG&"
  "%%-\211*Y\0L\10\275LG\330\343 M\15\275LG\266,\211\222(\211\246\5N\14\275L"
  "G\246MJ\42mZ\0O\12\275L\317\222yK\26\0P\14\275L\307\220d\332\240\204E\0Q\13"
  "\275L\317\222\271$R\244\4R\15\275L\307\220d\332\240\224*Y\0S\13\275L\317\222\251\253\226,"
  "\0T\11\275L\307\245\24v\2U\11\275LG\346[\262\0V\13\275LG\346\226\224\222,\2W\13"
  "\275LG\346\222(\211\322\5X\13\275LG\246%\265JM\13Y\12\275LG\246%\265\260\11Z\12"
  "\275L\307 f\35\7\1[\10;M\307\20u\32\134\7\255TG\332\1]\10;M\307\324i\10^"
  "\10\235lW\226\324\2_\6\215L\307 `\10#e\207\42e\1a\12\255L\217\32-Q\64\4b"
  "\13\275LGX\61i\223\242\0c\12\255L\317\222\211Y\262\0d\12\275Lg\305\264I\212\22e\12"
  "\255L\317\222\15C\272\0f\12<MW\245\64e%\0g\14\265D\317\22M\212\22&\13\0h\12"
  "\275LGX\61i\266\0i\10;MO(\265\14j\13\274L_\16d\231\224(\0k\13\274LG"
  "VR\22))\5l\10;M\207\324\313\0m\13\255L\207\322\242$J\242\24n\11\255LGb\322"
  "l\1o\11\255L\317\222\331\222\5p\13\265DGb\232\24%\14\1q\12\265D\217iR\224\260\0"
  "r\11\255LGb\22\213\0s\11\255L\317\240\36\24\0t\14\275LW\30\15RX\311\22\0u\11"
  "\255LG\346\244(\1v\12\255LGfKj\21\0w\12\255LGfI\224.\0x\12\255LG"
  "\226\324*\265\0y\14\265DG\246%C\250%\13\0z\11\255L\307\240\265\15\2{\12;MW\22"
  "%Y\224\5|\7\301E\307\62\4}\13;MG\26eI\224D\0~\11\235lO\226\324\22\0\177"
  "\10\305D\307\377\17\1\200\14\275L\227\24\15Y\266E\221\2\201\12\275LO\71sR\224\0\202\13\275"
  "L\237\274d\303\220\16\1\203\14\275L\307 \253\321\22EC\0\204\14\275LO\35P\243%\212\206\0"
  "\205\14\275L\207\16\251\321\22EC\0\206\14\275LW\216\250\321\22EC\0\207\13\265L\317\260\211C"
  "\226)\0\210\14\275L\307 /\331\60\244C\0\211\14\275LG\26/\331\60\244C\0\212\14\275L\207"
  "\16-\331\60\244C\0\213\11;MG\222I-\3\214\11\274L\217\22\245Zi\215\11\274L\207\16h"
  "M\3\216\14\275LG\26eIm\30\62-\217\14\275LW\16eIm\30\262\0\220\12\274L\227\70"
  "hK\66\4\221\13\255L\307\220%\311R\33\2\222\15\275L\327\322\224\14S\22%\221\0\223\14\275L"
  "W\226\344\300\222i\311\2\224\13\275LG\26/\231-Y\0\225\13\265L\207\16-\231\226,\0\226\13"
  "\275LW\226\304\231IQ\2\227\12\265L\207\216d&E\11\230\14DEG\24F\246d\33\22\0\231"
  "\13\275LG\226,\231[\262\0\232\12\275LG\226fn\311\2\233\16\305DW\230\15J\224dC\24"
  "F\0\234\14\275L\227\224(\321\220\205\331 \235\16\305D\207\62$\312A\212\6)\214\0\236\17\305D"
  "\307T\211\222\251\222\14Q\22%\0\237\15\305D\237\224D\331\26FI\244\1\240\13\275L\237\254FK"
  "\24\15\1\241\10\263M\217&U\6\242\12\265L\237\274dZ\262\0\243\11\265L\237\232\231\24%\244\11"
  "\64M\307\20.\221)\245\14\275L\307\240JC\242\14\321\26\246\14\265T\317\22%Q\64\244\203\0\247"
  "\12\265T\317\222i\311<\10\250\14\275LW\16eR\230%\13\0\251\6\225T\307\21\252\7\225T\307"
  " \6\253\13\305DG\230IYe\253\15\254\13\305DG\230IY\311\64\6\255\7\271M\207\62\4\256"
  "\12\255TW\322[\222%\1\257\13\255TG\222%Y\322\23\0\260\14>LOsRG\232\223\12\0"
  "\261\21FDGRJ\22\245\224$J)I\224RR\262\20FD\307\245\62\14Ie\30\222\312\60$"
  "\15\263\6AF\307A\264\10EDg\343\60\66\265\12EDg\70\214\303\330\0\266\21FD_\222%"
  "Y\222%\203\222%Y\222%\1\267\12&D\307-\311\222,\11\270\11\65D\307 \16c\3\271\20F"
  "D_\222%\203\222\16J\226dI\226\4\272\10\303EG\342/\1\273\14\66D\307uP\262$K\262"
  "$\274\14.\134_\222%\203\222\16C\0\275\14.\134_\222%Y\222%\303\20\276\11-\134g\70\214"
  "\303\0\277\7%D\307 \66\300\7*^G\322\42\301\10.\134g\332\62\14\302\10&D\307\61\255\0"
  "\303\11BFG\322\262T\0\304\6\16\134\307\1\305\12FDg\332\62\214i\5\306\11BFG\222\134"
  "*\0\307\10\303EG\342/\1\310\7\253]G\342\64\311\11\263E\307\20%.\1\312\13.\134_\222"
  "%\203\22\17\3\313\14\66D\307yH\262$K\262$\314\11\303EG\342\224\270\4\315\7\36\134\307y"
  "\30\316\20FD_\222%\203\22\17I\226dI\226\4\317\11.\134g\232\14\363\60\320\14.\134_\222"
  "%Y\222%\303\20\321\12\66D\307y\30\323\12\0\322\12&D\307-\311\222,\11\323\10\253]G\342"
  "\62\4\324\10*^G\222,\2\325\10\62F\307\262T\0\326\10\243E\307\220X\2\327\21FD_\222"
  "%Y\222%\303\220%Y\222%\1\330\14FDg\232\14c\62\214i\5\331\7-\134g\343\60\332\7"
  "\42F\307R\1\333\7FD\307\377\177\334\6&D\307\177\335\6CD\307\177\336\6\303E\307\177\337\6"
  "&d\307\177\340\13\255L\217)\211\222(R\2\341\14\275D\317\222)JfQB\0\342\10\275L\307"
  "i\354\10\343\13\255L\307\240\224\222()\11\344\12\275L\307-I\263\332\60\345\14\255L\317 %Q"
  "\22E\22\0\346\14\265DO)\211\222(\211\224\15\347\11\255L\317\240\204\65\11\350\15\305D\307 e"
  "KE\251l\321 \351\15\275LW\226\324\206!Kj\21\0\352\14\275LW\226\324\264\244\224$\6\353"
  "\14\275L\227\224JK\246%\13\0\354\11\235\134O\262$K\2\355\16\305Dg\262DK\242$S\262"
  "\204\0\356\12\274L\317\220e\203\26\16\357\10\275L\317\222\371\26\360\11\255T\307\240\16\352 \361\14\275"
  "LW\30\15R\230\3\203\0\362\12\275LOZ\253#\203\0\363\10\275L_ky\20\364\11\303E\307"
  "\220H=\1\365\11\303DW'E\31\2\366\13\275L\227&\17:\240)\0\367\11\255T\307\242\254\213"
  "\62\370\11$e\217\22I\211\2\371\6\222]\307\20\372\6\212]\207\0\373\14\305D\327\24\226\222(\311"
  "\304\10\374\10,]\307\22\71\5\375\11,]\217\22\65\15\1\376\7$U\307\17\1\377\5\0D\7\0"
  "\0\0\4\377\377\0";






#endif
