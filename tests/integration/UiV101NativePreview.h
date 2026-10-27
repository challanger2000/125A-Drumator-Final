#pragma once
// Native VSTGUI DESIGN PREVIEW: separate from the released Drumator editor.
// Receives real Controller-derived data; layout and live mouse input are separate.
#include "core/UiV101Layout.h"
#include "vst3/UiV101Interaction.h"
#include "vstgui/lib/cdrawcontext.h"
#include "vstgui/lib/cgraphicspath.h"
#include "vstgui/lib/cgraphicstransform.h"
#include "vstgui/lib/cbitmap.h"
#include "vst3/BrandLogoGeometry.h"
#include <cstdlib>
#include <array>
#include <cmath>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdint>

namespace drumator::qa {
struct UiV101NativePreview final {
 using L=UiV101Layout;
 using Color=VSTGUI::CColor;
 using Box=VSTGUI::CRect;
 static constexpr double kRowMountRadius=23.;
 static constexpr double kHeaderMountRadius=22.;
 static_assert(2.*kRowMountRadius+2.<=L::rowHeight,
               "Rotary mounting socket must not overlap the next 48px row");
 static_assert(L::headerKnobY-kHeaderMountRadius>=97.&&
               L::headerKnobY+kHeaderMountRadius<=L::header-17.,
               "Header mounting socket must stay clear of the heading and ruler");
 // Material colors sampled from the supplied reference screenshot. The
 // original light steel is warm (R>G>B), and the sequencer is dark navy,
 // deliberately NOT Windows grey or near-black.
 struct Skin {
   static Color steel(){return {201,194,183,255};}
   static Color steelHighlight(){return {217,209,197,255};}
   static Color steelShadow(){return {171,164,155,255};}
   static Color headerWell(){return {197,190,179,255};}
   static Color headerSurface(){return {207,200,189,255};}
   static Color graphite(){return {29,32,36,255};}
   static Color panelEdge(){return {101,93,86,255};}
   static Color navyWell(){return {15,27,40,255};}
   static Color navyA(){return {27,41,56,255};}
   static Color navyB(){return {30,45,61,255};}
   static Color navyTrim(){return {63,78,92,255};}
   static Color stepOff(){return {36,51,70,255};}
   static Color amber(){return {250,145,30,255};}
   static Color amberLens(){return {255,195,74,255};}
 };
 struct RowData final {
   std::string name;
   bool hasSample=false,muted=false,solo=false;
   unsigned gain=100;
   int pan=0,pitch=0;
   std::array<std::uint8_t,L::steps> velocities{};
 };
 struct Scene final {
   std::size_t pattern=0,visibleRows=L::visibleRows;
   std::uint8_t patternLength=32;
   bool songEnabled=false;
   unsigned swing=0,humanize=0,master=100;
   std::array<RowData,L::instruments> rows{};
 };
 static void fill(VSTGUI::CDrawContext& c,Box r,Color color){
   c.setFillColor(color);c.drawRect(r,VSTGUI::kDrawFilled);
 }
 static void border(VSTGUI::CDrawContext& c,Box r,Color color){
   c.setFrameColor(color);c.setLineWidth(1.);c.drawRect(r,VSTGUI::kDrawStroked);
 }
 static Color mix(Color a,Color b,double t){
   t=std::clamp(t,0.,1.);
   return {static_cast<std::uint8_t>(std::lround(a.red+(b.red-a.red)*t)),
           static_cast<std::uint8_t>(std::lround(a.green+(b.green-a.green)*t)),
           static_cast<std::uint8_t>(std::lround(a.blue+(b.blue-a.blue)*t)),255};
 }
 // Deterministic native paint: two-slope satin-metal gradient without a
 // bitmap dependency. Evenly spaced strips give subtle directionality at
 // both 100% and 150% while keeping repaint costs bounded.
 static void satin(VSTGUI::CDrawContext& c,Box r,Color upper,Color middle,
                   Color lower,int strips=20){
   if(r.getWidth()<=0.||r.getHeight()<=0.||strips<1)return;
   for(int i=0;i<strips;++i){
     const double top=r.top+r.getHeight()*i/strips;
     const double bottom=r.top+r.getHeight()*(i+1)/strips;
     const double t=(i+.5)/strips;
     const auto ink=t<.5?mix(upper,middle,t*2.):
                            mix(middle,lower,(t-.5)*2.);
     fill(c,{r.left,top,r.right,bottom},ink);
   }
 }
 // Square buttons were the main Windows-98 cue. These tiny inset corner
 // cuts mimic a physical rounded metal key, without changing mouse hits.
 static void rounded(VSTGUI::CDrawContext& c,Box r,Color color,double corner=3.){
   if(r.getWidth()<=corner*2.||r.getHeight()<=corner*2.)return;
   fill(c,{r.left+corner,r.top,r.right-corner,r.bottom},color);
   fill(c,{r.left+1.,r.top+1.,r.right-1.,r.bottom-1.},color);
   fill(c,{r.left,r.top+corner,r.right,r.bottom-corner},color);
 }
 // Dark screw heads with a brushed rim, one highlight and cross recess.
 // These are paint-only and do not intrude into interactive rectangles.
 static void screw(VSTGUI::CDrawContext& c,double x,double y){
   c.setFillColor({69,63,58,255});
   c.drawEllipse({x-7.,y-7.,x+7.,y+7.},VSTGUI::kDrawFilled);
   c.setFrameColor({226,220,210,255});c.setLineWidth(1.);
   c.drawEllipse({x-6.,y-6.,x+6.,y+6.},VSTGUI::kDrawStroked);
   c.setFillColor({28,28,27,255});
   c.drawEllipse({x-4.5,y-4.5,x+4.5,y+4.5},VSTGUI::kDrawFilled);
   c.setFrameColor({122,117,111,255});
   c.drawLine({x-2.5,y-2.5},{x+2.5,y+2.5});
   c.drawLine({x+2.5,y-2.5},{x-2.5,y+2.5});
   c.setFrameColor({13,13,14,255});
   c.drawLine({x-2.5,y-1.5},{x+1.5,y+2.5});
 }
 static void label(VSTGUI::CDrawContext& c,const char* value,Box r,
                   int font=11,Color ink={191,209,223,255},bool center=true){
   c.setFont(VSTGUI::kNormalFont,font);c.setFontColor(ink);
   c.drawString(value,r,center?VSTGUI::kCenterText:VSTGUI::kLeftText);
 }
 // Exactly the V1.0 official 125A master geometry, never a font redraw.
 // Only its uniform scale and position change to fit the ivory fascia.
 static void brandLogo(VSTGUI::CDrawContext& c,double left,double top,double targetHeight){
   const double sf=targetHeight/887.;
   for(const auto& shape:kBrandLogoPaths){
     auto* path=c.createGraphicsPath();
     if(!path)continue;
     const char* cursor=shape.path;
     while(*cursor){
       while(*cursor==' '||*cursor=='\n'||*cursor=='\t')++cursor;
       if(!*cursor)break;
       const char command=*cursor++;
       if(command=='Z'){path->closeSubpath();continue;}
       if(command!='M'&&command!='L')break;
       while(*cursor==' ')++cursor;
       char* end=nullptr;
       const double x=std::strtod(cursor,&end);
       if(end==cursor)break;
       cursor=end;
       while(*cursor==' ')++cursor;
       const double y=std::strtod(cursor,&end);
       if(end==cursor)break;
       cursor=end;
       if(command=='M')path->beginSubpath(left+x*sf,top+y*sf);
       else path->addLine(left+x*sf,top+y*sf);
     }
     c.setFillColor(shape.red?Color{215,25,32,255}:Color{222,218,209,255});
     c.drawGraphicsPath(path,VSTGUI::CDrawContext::kPathFilledEvenOdd);
     path->forget();
   }
 }
 // Draw the EXACT product-title PNG used by V1.0 at native aspect ratio.
 static void titleBitmap(VSTGUI::CDrawContext& c,VSTGUI::CBitmap* logo){
   if(!logo||logo->getWidth()<=0.||logo->getHeight()<=0.)return;
   constexpr double boxHeight=85.;
   const double sf=boxHeight/logo->getHeight();
   const double w=logo->getWidth()*sf;
   VSTGUI::CDrawContext::Transform tx(c,
     VSTGUI::CGraphicsTransform().scale(sf,sf).translate((1280.-w)/2.,-9.));
   logo->draw(&c,{0.,0.,logo->getWidth(),logo->getHeight()});
 }
 // Same 125x125, 101-frame JKnobMan image used by V1.0; the actual
 // opaque dial sits inside its transparent frame. Not a painted imitation.
 static void originalDial(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* sprite,
                          double cx,double cy,double fraction,double frameSize){
   if(!sprite||sprite->getWidth()!=125.||sprite->getHeight()!=12625.)return;
   const double sf=frameSize/125.;
   const double left=cx-frameSize/2.,top=cy-frameSize/2.;
   const auto frame=sprite->normalizedValueToFrameIndex(
     static_cast<float>(std::clamp(fraction,0.,1.)));
   VSTGUI::CDrawContext::Transform tx(c,
     VSTGUI::CGraphicsTransform().scale(sf,sf).translate(left,top));
   sprite->drawFrame(&c,frame,{0.,0.});
 }
 // Shared machined keycap family. Every action button uses the same
 // rim, satin-silver/amber lens, highlights, lower bevel, lettering and shadow.
 // All layers sit strictly inside the common paint/hit rectangle.
 static void cap(VSTGUI::CDrawContext& c,L::Rect r,double y,const char* text,
                 bool active=false,int font=10){
   const Box b{r.x0,y+r.y0,r.x1,y+r.y1};
   const double l=b.left,t=b.top,rr=b.right,bt=b.bottom;
   const bool compact=b.getHeight()<24.;
   const double radius=compact?2.:3.;
   // Deep charcoal pocket + soft offset shadow under the raised cap.
   rounded(c,{l,t+2.,rr,bt+2.},{7,10,14,255},radius);
   rounded(c,{l,t,rr,bt-1.},active?
             Color{116,56,21,255}:Color{30,31,33,255},radius);
   rounded(c,{l+1.,t+1.,rr-1.,bt-3.},active?
             Color{255,169,47,255}:Color{106,101,94,255},radius);
   rounded(c,{l+2.,t+2.,rr-2.,bt-4.},active?
             Color{177,72,20,255}:Color{179,171,160,255},radius);
   // A mild warm facet gradient; do not let the controls look chrome-white.
   satin(c,{l+4.,t+4.,rr-4.,bt-6.},
         active?Color{255,224,122,255}:Color{226,219,208,255},
         active?Color{244,151,46,255}:Color{197,190,180,255},
         active?Color{217,94,24,255}:Color{158,151,143,255},compact?3:7);
   // Continuous edge lights make the pressed vs released geometry legible.
   fill(c,{l+5.,t+3.,rr-5.,t+4.},active?
        Color{255,235,158,255}:Color{248,240,226,255});
   fill(c,{l+5.,bt-6.,rr-5.,bt-5.},active?
        Color{151,53,13,255}:Color{110,104,98,255});
   fill(c,{l+2.,t+5.,l+3.,bt-6.},active?
        Color{255,189,89,255}:Color{237,228,214,255});
   fill(c,{rr-3.,t+5.,rr-2.,bt-6.},active?
        Color{132,49,17,255}:Color{85,80,77,255});
   label(c,text,{l+2.,t+2.,rr-2.,bt-2.},font,
         active?Color{65,32,16,255}:Color{24,27,30,255});
 }
 // Dark-blue physical sequencer keycap, with matching bezel and warm
 // backlit orange state. Keep the same 17px step width / 19px pitch.
 // Faithful V1.0 drawHardwareStep paint translated into compact 48px row.
 // Original established dark face, quarter accent and lit amber key lens.
 static void stepCap(VSTGUI::CDrawContext& c,double sx,double y,bool lit,bool downbeat){
   // This is a compact 17x27 physical switch INSIDE the established
   // 17x31 hit surface. Slightly rounded edges, black gasket, recess shadow.
   const double x=sx+1.;
   if(lit){
     // Restrained warm halo is visible against navy, not an orange rectangle.
     fill(c,{x,y+12.,x+17.,y+38.},{70,42,22,255});
   }
   rounded(c,{x,y+10.,x+17.,y+39.},{6,11,18,255},2.);
   rounded(c,{x+1.,y+11.,x+16.,y+37.},
           downbeat?Color{94,111,129,255}:Color{46,64,83,255},2.);
   rounded(c,{x+2.,y+12.,x+15.,y+36.},
           lit?Color{211,71,10,255}:Color{29,43,62,255},2.);
   satin(c,{x+3.,y+14.,x+14.,y+34.},
         lit?Color{255,206,91,255}:Color{86,105,124,255},
         lit?Color{252,150,35,255}:Color{47,67,90,255},
         lit?Color{229,86,14,255}:Color{23,38,55,255},9);
   fill(c,{x+4.,y+13.,x+13.,y+15.},
        lit?Color{255,235,145,255}:Color{125,140,156,255});
   fill(c,{x+4.,y+34.,x+13.,y+35.},
        lit?Color{166,54,9,255}:Color{13,26,41,255});
   if(lit)fill(c,{x+5.,y+28.,x+12.,y+31.},{255,198,79,255});
 }
 // Headphone pictogram replaces the ambiguous play triangle in PREVIEW.
 // All coordinates derive from the preview cap's shared hit/paint rectangle.
 static void headphone(VSTGUI::CDrawContext& c,L::Rect r,double y){
   cap(c,r,y,"");
   const double cx=(r.x0+r.x1)/2.,cy=y+(r.y0+r.y1)/2.;
   c.setFrameColor({30,35,43,255});c.setLineWidth(2.5);
   // Rounded headband arc, mirrored for even balance at 100% zoom.
   constexpr std::array<double,9> arc{{-1.,-.96,-.80,-.52,0.,.52,.80,.96,1.}};
   for(std::size_t i=1;i<arc.size();++i){
     const auto px=[&](double t){return cx+10.*t;};
     const auto py=[&](double t){return cy-1.-10.*std::sqrt(1.-t*t);};
     c.drawLine({px(arc[i-1]),py(arc[i-1])},{px(arc[i]),py(arc[i])});
   }
   c.setFillColor({28,35,43,255});
   c.drawRect({cx-12.,cy-2.,cx-8.,cy+9.},VSTGUI::kDrawFilled);
   c.drawRect({cx+8.,cy-2.,cx+12.,cy+9.},VSTGUI::kDrawFilled);
   c.setFrameColor({105,110,115,255});c.setLineWidth(1.);
   c.drawLine({cx-9.,cy-1.},{cx-9.,cy+7.});
   c.drawLine({cx+9.,cy-1.},{cx+9.,cy+7.});
 }
 // Smaller hardware rotary with a steel mounting ring, scale and brushed face.
 // Used for the three actual global parameters, not painted as buttons.
 // Actual 125A knob sprite, reused for the two global musical controls
 // and master, with legible readouts and small engraved arc ticks.
 static void headerDial(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* sprite,
                        double cx,double cy,double fraction,
                        const char* title,const char* readout){
   fraction=std::clamp(fraction,0.,1.);
   label(c,title,{cx-46.,85.,cx+46.,100.},11,{55,56,58,255});
   // Small recessed socket; must not cover SWING/HUMANIZE/MASTER labels.
   c.setFillColor({62,58,53,255});
   c.drawEllipse({cx-kHeaderMountRadius,cy-kHeaderMountRadius,cx+kHeaderMountRadius,cy+kHeaderMountRadius},VSTGUI::kDrawFilled);
   c.setFrameColor({238,230,214,255});c.setLineWidth(1.);
   c.drawEllipse({cx-21.,cy-21.,cx+21.,cy+21.},VSTGUI::kDrawStroked);
   c.setFillColor({12,15,20,255});
   c.drawEllipse({cx-19.,cy-19.,cx+19.,cy+19.},VSTGUI::kDrawFilled);
   c.setFrameColor({126,115,103,255});
   c.drawEllipse({cx-18.,cy-18.,cx+18.,cy+18.},VSTGUI::kDrawStroked);
   constexpr double pi=3.14159265358979323846;
   for(int tick=0;tick<9;++tick){
     const double angle=(-135.+static_cast<double>(tick)*270./8.)*pi/180.;
     const double dx=std::sin(angle),dy=-std::cos(angle);
     c.setFrameColor({64,66,68,255});
     c.drawLine({cx+dx*22.,cy+dy*22.},{cx+dx*24.,cy+dy*24.});
   }
   originalDial(c,sprite,cx,cy,fraction,55.);
   label(c,readout,{cx+26.,108.,cx+54.,133.},10,{45,48,52,255},false);
 }
 static void dial(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* sprite,
                  double x,double y,int value,int index){
   const double fraction=index==0?value/200.:index==1?(value+100.)/200.:
                         (value+24.)/48.;
   // Radius < 24px: mounting rings never touch adjacent 48px rows.
   c.setFillColor({8,16,24,255});
   c.drawEllipse({x-kRowMountRadius,y-kRowMountRadius,x+kRowMountRadius,y+kRowMountRadius},VSTGUI::kDrawFilled);
   c.setFrameColor({92,100,108,255});c.setLineWidth(1.);
   c.drawEllipse({x-22.,y-22.,x+22.,y+22.},VSTGUI::kDrawStroked);
   c.setFillColor({7,11,17,255});
   c.drawEllipse({x-20.,y-20.,x+20.,y+20.},VSTGUI::kDrawFilled);
   originalDial(c,sprite,x,y,std::clamp(fraction,0.,1.),50.);
 }
 static void draw(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* knobSprite,
                  VSTGUI::CBitmap* productTitle,const Scene& scene,
                  bool zoomMenuOpen=false,std::size_t zoomPreset=0,
                  const UiV101Interaction::State& ui={}){
   const auto cream=Skin::steel();
   auto expanded=L::Expansions{};
   if(ui.expandedRow<L::instruments)expanded[ui.expandedRow]=ui.expansion;
   const auto limit=ui.expansion==L::Expansion::None?
                    L::visibleRows:L::visibleRows-1;
   const auto shown=ui.firstRow<scene.visibleRows?
                    std::min(limit,scene.visibleRows-ui.firstRow):std::size_t{0};
   const double total=L::editorHeight(ui.firstRow,shown,expanded);
   fill(c,{0.,0.,L::width,total},{105,98,91,255});
   rounded(c,{2.,2.,L::width-2.,total-2.},{165,157,148,255},3.);
   satin(c,{5.,5.,L::width-5.,total-5.},
         {214,208,200,255},cream,{184,177,167,255},72);
   border(c,{5.,5.,L::width-5.,total-5.},Skin::panelEdge());
   // Subpixel-style horizontal brushing is deliberately faint (alpha ~22).
   // Highlights change with vertical position; no generated bitmap needed.
   c.setLineWidth(1.);
   for(int yy=14;yy<static_cast<int>(total)-13;yy+=5){
     c.setFrameColor({241,235,226,9});
     c.drawLine({12.,static_cast<double>(yy)},
                {L::width-12.,static_cast<double>(yy)});
   }
   fill(c,{7.,7.,L::width-7.,9.},Skin::steelHighlight());
   fill(c,{7.,total-9.,L::width-7.,total-7.},{157,148,139,255});
   screw(c,14.,15.);screw(c,L::width-14.,15.);
   screw(c,14.,total-15.);screw(c,L::width-14.,total-15.);
   // Original 125A SVG geometry on a dark enamel badge, like the supplied
   // hardware reference. Only badge treatment / SVG fill change, not paths.
   rounded(c,{26.,10.,157.,76.},{34,31,30,255},5.);
   rounded(c,{28.,12.,155.,73.},{124,115,103,255},4.);
   rounded(c,{30.,14.,153.,71.},{25,27,29,255},4.);
   fill(c,{35.,15.,148.,16.},{141,137,132,255});
   brandLogo(c,32.,12.,55.);
   titleBitmap(c,productTitle);
   // The percentage and dropdown affordance identify the single zoom control;
   // no redundant "ZOOM" legend or decorative separating line.
   // The menu itself is painted as an
   // overlay after all rows, as a real hardware-style popover would be.
   const char* const zoomNames[]={"100%","120%","130%","150%"};
   const auto selected=zoomPreset<4?zoomPreset:std::size_t{0};
   char zoomCaption[20]{};
   std::snprintf(zoomCaption,sizeof(zoomCaption),"%s  v",zoomNames[selected]);
   cap(c,L::zoomTrigger,0.,zoomCaption,zoomMenuOpen,11);
   // Actual Drumator features only. This header is not yet interactive.
   rounded(c,{21.,82.,1260.,141.},{117,108,99,255},4.);
   rounded(c,{23.,84.,1258.,138.},Skin::headerWell(),3.);
   satin(c,{26.,86.,1255.,136.},
         {216,210,199,255},Skin::headerSurface(),
         {186,179,169,255},18);
   fill(c,{28.,85.,1253.,86.},{237,229,216,255});
   fill(c,{28.,136.,1253.,137.},{113,105,98,255});
   border(c,{22.,83.,1259.,139.},Skin::panelEdge());
   // Engraved panel divisions, painted behind all interactive controls.
   for(const double xx:{350.,521.,729.,1115.}){
     fill(c,{xx,86.,xx+1.,136.},{151,141,131,255});
     fill(c,{xx+1.,86.,xx+2.,136.},{227,218,206,255});
   }
   label(c,"PATTERN",{33.,85.,154.,104.},11,{51,54,57,255},false);
   for(int i=0;i<8;++i){
     const char key[2]={static_cast<char>('A'+i),'\0'};
     L::Rect r{33.+i*39.,29.,68.+i*39.,63.};
     cap(c,r,78.,key,i==static_cast<int>(scene.pattern),12);
   }
   label(c,"PTNR",{356.,85.,412.,104.},10,{54,54,54,255});
   // The two SONG functions are buttons; global tonal controls stay knobs.
   cap(c,{364.,29.,414.,63.},78.,"MENU");
   cap(c,{431.,29.,514.,63.},78.,scene.patternLength==32?"2 BARS":"1 BAR");
   cap(c,L::headerGroup(0),0.,"SONG",scene.songEnabled,11);
   cap(c,L::headerGroup(1),0.,"SONG EDIT",false,10);
   char swingText[18]{},humanText[18]{},masterText[18]{};
   std::snprintf(swingText,sizeof(swingText),"%u%%",scene.swing);
   std::snprintf(humanText,sizeof(humanText),"%u%%",scene.humanize);
   std::snprintf(masterText,sizeof(masterText),"%u%%",scene.master);
   headerDial(c,knobSprite,L::headerKnobCenters[0],L::headerKnobY,scene.swing/50.,
              "SWING",swingText);
   headerDial(c,knobSprite,L::headerKnobCenters[1],L::headerKnobY,scene.humanize/100.,
              "HUMANIZE",humanText);
   headerDial(c,knobSprite,L::headerKnobCenters[2],L::headerKnobY,scene.master/200.,
              "MASTER",masterText);
   fill(c,{1132.,109.,1146.,129.},{20,30,41,255});
   fill(c,{1151.,105.,1160.,129.},{29,54,65,255});
   fill(c,{1164.,111.,1174.,129.},{29,62,82,255});
   label(c,"L / R",{1181.,107.,1240.,126.},11,{49,54,58,255});
   // Column headings remain outside the 48px-high channels.
   fill(c,{12.,142.,1261.,157.},Skin::navyWell());
   fill(c,{12.,142.,1261.,143.},Skin::navyTrim());
   label(c,"M",{20.,140.,47.,157.},10);
   label(c,"S",{52.,140.,79.,157.},10);
   label(c,"FILL",{84.,140.,124.,157.},10);
   label(c,"INSTRUMENT / SAMPLE",{132.,140.,304.,157.},10);
   label(c,"PREV",{310.,140.,347.,157.},10);
   label(c,"EDIT",{354.,140.,406.,157.},10);
   for(int i=0;i<32;i+=4){
     char n[8]{};std::snprintf(n,sizeof(n),"%d",i+1);
     label(c,n,{L::stepX+i*L::stepPitch,140.,L::stepX+(i+2)*L::stepPitch,157.},10);
   }
   label(c,"VOL",{1037.,140.,1087.,157.},10);
   label(c,"PAN",{1097.,140.,1147.,157.},10);
   label(c,"PITCH",{1153.,140.,1214.,157.},10);
   for(std::size_t index=0;index<shown;++index){
     const std::size_t row=ui.firstRow+index;
     const double y=L::rowTop(ui.firstRow,index,expanded);
     const auto& data=scene.rows[row];
     const bool live=row<scene.visibleRows;
     // Recessed dark-navy instrument chassis: thick top gasket, soft
     // internal gradient, narrow upper reflection and lower occlusion.
     rounded(c,{12.,y+1.,1260.,y+L::rowHeight-1.},{7,15,25,255},3.);
     rounded(c,{14.,y+3.,1258.,y+L::rowHeight-3.},
             row%2?Skin::navyA():Skin::navyB(),2.);
     satin(c,{17.,y+5.,1255.,y+L::rowHeight-5.},
           {44,61,76,255},row%2?Skin::navyA():Skin::navyB(),
           {19,32,48,255},12);
     fill(c,{18.,y+4.,1254.,y+5.},{84,100,111,255});
     fill(c,{18.,y+L::rowHeight-5.,1254.,y+L::rowHeight-4.},
          {9,17,28,255});
     // Slim selection rail: visible without adding another chunky module.
     fill(c,{L::select.x0,y+L::select.y0,L::select.x1,y+L::select.y1},
          {5,12,19,255});
     fill(c,{L::select.x0+3.,y+14.,L::select.x1-3.,y+33.},
          live&&row==ui.selectedRow?Color{246,171,70,255}:Color{76,93,103,255});
     cap(c,L::mute,y,"M",live&&data.muted);
     cap(c,L::solo,y,"S",live&&data.solo);
     cap(c,L::fill,y,"FILL",false,9);
     rounded(c,{L::sample.x0,y+8.,L::sample.x1,y+40.},{3,8,14,255},2.);
     satin(c,{L::sample.x0+2.,y+10.,L::sample.x1-2.,y+38.},
           {6,14,23,255},{11,22,35,255},{18,34,49,255},9);
     fill(c,{L::sample.x0+3.,y+10.,L::sample.x1-3.,y+11.},
          {71,88,101,255});
     border(c,{L::sample.x0,y+8.,L::sample.x1,y+40.},{62,82,98,255});
     cap(c,L::previous,y,"<",false,12);
     cap(c,L::next,y,">",false,12);
     const std::string emptyName="(EMPTY)";
     const auto& rowName=live?(data.name.empty()?emptyName:data.name):emptyName;
     label(c,rowName.c_str(),{L::load.x0+5.,y+9.,L::load.x1-2.,y+39.},
           11,live?Color{203,222,235,255}:Color{103,116,127,255},false);
     headphone(c,L::preview,y);
     cap(c,L::edit,y,"EDIT",false,11);
     rounded(c,{L::stepWell.x0,y+4.,L::stepWell.x1,y+44.},
             {9,17,29,255},2.);
     fill(c,{L::stepWell.x0+2.,y+6.,L::stepWell.x1-2.,y+8.},
          {8,15,26,255});
     fill(c,{L::stepWell.x0+2.,y+41.,L::stepWell.x1-2.,y+42.},
          {55,71,88,255});
     for(std::size_t step=0;step<L::steps;++step){
       const auto r=L::stepRect(step);
       const double sx=r.x0;
       const bool active=live&&step<scene.patternLength&&data.velocities[step]>0;
       stepCap(c,sx,y,active,step%4==0);
     }
     for(std::size_t k=0;k<L::knobCenters.size();++k)
       dial(c,knobSprite,L::knobCenters[k],y+24.,
            k==0?static_cast<int>(data.gain):k==1?data.pan:data.pitch,
            static_cast<int>(k));
     cap(c,L::velocity,y,"VEL",ui.expandedRow==row&&
           ui.expansion==L::Expansion::Velocity,9);
     cap(c,L::settings,y,"SET",ui.expandedRow==row&&
           ui.expansion==L::Expansion::Settings,9);
     if(ui.expandedRow==row&&ui.expansion!=L::Expansion::None){
       fill(c,{13.,y+49.,1260.,y+95.},Skin::navyWell());
       fill(c,{15.,y+50.,1258.,y+94.},Skin::navyA());
       if(ui.expansion==L::Expansion::Velocity){
         label(c,"VELOCITY",{132.,y+55.,265.,y+88.},11,{213,227,237,255},false);
         fill(c,{417.,y+53.,1031.,y+92.},{4,13,25,255});
         for(std::size_t step=0;step<L::steps;++step){
           const auto velocity=step<scene.patternLength?
                               data.velocities[step]:std::uint8_t{0};
           const double xx=L::stepX+step*L::stepPitch;
           if(velocity==0){
             fill(c,{xx+2.,y+88.,xx+15.,y+90.},{37,56,75,255});
           }else{
             const double barHeight=3.+32.*static_cast<double>(velocity)/127.;
             fill(c,{xx+2.,y+89.-barHeight,xx+15.,y+89.},
                  {245,139,40,255});
             fill(c,{xx+3.,y+87.-barHeight,xx+14.,y+89.-barHeight},
                  {255,201,99,255});
           }
         }
       }else{
         constexpr std::array<const char*,6> settingsLabels{{
           "OUT","CHOKE","COPY ROW","PASTE ROW","CLEAR ROW","RANDOM VEL"
         }};
         for(std::size_t button=0;button<settingsLabels.size();++button)
           cap(c,UiV101Interaction::settingsRect(button),y,
               settingsLabels[button],false,10);
       }
     }
   }
   fill(c,{14.,total-44.,1260.,total-12.},Skin::headerSurface());
   border(c,{14.,total-44.,1260.,total-12.},Skin::panelEdge());
   char summary[160]{};
   std::snprintf(summary,sizeof(summary),
                 "INSTRUMENTS %zu-%zu / %zu   |   PATTERN %c   |   NATIVE CONTROLLER STATE",
                 ui.firstRow+1,ui.firstRow+shown,scene.visibleRows,
                 static_cast<char>('A'+scene.pattern));
   label(c,summary,{31.,total-42.,922.,total-14.},11,
         {66,68,70,255},false);
   cap(c,{975.,7.,1095.,38.},total-L::footer,"PREV PAGE",
         ui.firstRow>0,10);
   cap(c,{1105.,7.,1235.,38.},total-L::footer,"NEXT PAGE",
         ui.firstRow+shown<scene.visibleRows,10);
   if(zoomMenuOpen){
     fill(c,{1130.,45.,1252.,155.},{12,20,29,255});
     border(c,{1130.,45.,1252.,155.},{104,115,121,255});
     for(std::size_t i=0;i<L::zoomPresets.size();++i){
       const auto rect=L::zoomOption(i);
       // Menu options use the same physical caps as the other GUI buttons.
       cap(c,rect,0.,zoomNames[i],i==selected,11);
     }
   }
 }
};
}
