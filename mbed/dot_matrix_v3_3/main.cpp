
#include "mbed.h"
#include <stdio.h>
#include <string>
#include "font8ex.h"

#define mLED_COL 8

const int mMaxStr=12;
int patterns[2][8][8];
const char mSpace[6+1]="e38080";

unsigned char pat16[mMaxStr][8];

int nPatternMax=0;
int mSTAT =0;
int mSTAT_RSV_START=1;
int mSTAT_RSV_END  =2;
int mSTAT_DISP=3;
int iMC=0;
int mHtyp_One=1;
int mHtyp_SCR=2;

string mReceive ="";
string mReceive2="";
string mResponse="";
string mHtypRecv="";

//pin
DigitalOut mPin_1( dp1);
DigitalOut mPin_2( dp2);
DigitalOut mPin_3( dp4);
DigitalOut mPin_4( dp6);
DigitalOut mPin_5( dp5);
DigitalOut mPin_6( dp9);
DigitalOut mPin_7( dp10 );
DigitalOut mPin_8( dp11 );

DigitalOut mPin_9( dp13);
DigitalOut mPin_10(dp14);
DigitalOut mPin_11(dp17);
DigitalOut mPin_12(dp18 );
DigitalOut mPin_13(dp25 );
DigitalOut mPin_14(dp27 );
DigitalOut mPin_15(dp26 );
DigitalOut mPin_16(dp28 );

DigitalOut mCols[8]={mPin_13, mPin_3, mPin_4, mPin_10, mPin_6, mPin_11, mPin_15, mPin_16};
DigitalOut mRows[8]={mPin_9, mPin_14, mPin_8, mPin_12, mPin_1, mPin_7 , mPin_2, mPin_5};

Serial mPc(USBTX, USBRX);

//
void buildMapCustom(unsigned char char1,  unsigned char char2, unsigned char char3 ){
    int iMax=0;
    int iPos=0;
    iMax=sizeof(font8ex_FF) / (mLED_COL+2);
    for(int i = 0; i < iMax; i++){
      iPos=i* 10;
      if(((unsigned char)font8ex_FF[iPos]== char2) && ((unsigned char)font8ex_FF[iPos+1]== char3))
      {
        for(int j = 0; j < mLED_COL ; j++){ pat16[nPatternMax][j]=(unsigned char)font8ex_FF[iPos+(j+2)];  }
        return;
      }
    }
}

//
 void buildMapCMP(unsigned char char1,  unsigned char char2, unsigned char char3 )
{
    int iMax=0;
    int iPos=0;
    if ((0xE2 <= char1) && (char1 <= 0xEF)){
         switch(char1 - 0xE2) {
            case  0:
                iMax=sizeof(font8ex_E2) / (mLED_COL+2);
//printf("iMax=%d\n" ,iMax);
                for(int i = 0; i < iMax; i++){
                    iPos=i* 10;
                    if(((unsigned char)font8ex_E2[iPos]== char2) && ((unsigned char)font8ex_E2[iPos+1]== char3))
                    {
//printf("e2=%x ,iPos=%d\n" ,font8ex_E2[iPos], iPos);
                        for(int j = 0; j < mLED_COL ; j++){ pat16[nPatternMax][j]=(unsigned char)font8ex_E2[iPos+(j+2)];  }
                    }
                }
                break;
            case  1:
                iMax=sizeof(font8ex_E3) / (mLED_COL+2);
                for(int i = 0; i < iMax; i++){
                    iPos=i* 10;
                    if(((unsigned char)font8ex_E3[iPos]== char2) && ((unsigned char)font8ex_E3[iPos+1]== char3))
                    {
//printf("e2=%x ,iPos=%d\n" ,font8ex_E3[iPos], iPos);
                        for(int j = 0; j < mLED_COL ; j++){ pat16[nPatternMax][j]=(unsigned char)font8ex_E3[iPos+(j+2)];  }
                    }
                }
                break;
            case  13:
                iMax=sizeof(font8ex_EF) / (mLED_COL+2);
                for(int i = 0; i < iMax; i++){
                    iPos=i* 10;
                    if(((unsigned char)font8ex_EF[iPos]== char2) && ((unsigned char)font8ex_EF[iPos+1]== char3))
                    {
//printf("e2=%x ,iPos=%d\n" ,font8ex_EF[iPos], iPos);
                        for(int j = 0; j < mLED_COL ; j++){ pat16[nPatternMax][j]=(unsigned char)font8ex_EF[iPos+(j+2)];  }
                    }
                }
                break;            
//printf("iMax=%d\n" ,iMax);
            default: for(int j = 0; j < mLED_COL ; j++){ pat16[nPatternMax][j]=0x00; }
         }
    }
         
//  char line[2+1];
//  for(int i = 0; i < sizeof(fErrMap); i++)fBitMap[i+2] = fErrMap[i];
}

//
unsigned int pow_get(int src, int iPnum){
    int ret=1;

    if(iPnum > 0){
        ret=src;
        for(int i=0; i<iPnum-1; i++){
          ret=ret * src;
        }
    }
    return ret;
}

unsigned int transUInt(unsigned char c){
    if('0'<=c && '9'>=c) return (c-0x30);//0x30は'0'の文字コード
    if('A'<=c && 'F'>=c) return (c+0x0A-0x41);//0x41は'A'の文字コード
    if('a'<=c && 'f'>=c) return (c+0x0A-0x61);//0x61は'a'の文字コード
    return 0;
}

//
unsigned int hexToUInt(char *str)
{
// printf("hexToUInt ,ptr=%s\n" , &str[0]);
    unsigned int i,j=0;
    char*str_ptr=str+strlen(str)-1;

    for(i=0;i<strlen(str);i++){
//printf("tr=%d\n" ,pow_get(16, i));
//printf("pow=%d\n" , (unsigned int)pow(16,i));
        j+=transUInt(*str_ptr--)*pow_get(16, i);
//printf("j=%d ,i=%d\n", j, i);
    }
    return j;
}
//
int drawHextext(string src){
  unsigned char c;
  int ret=0;
  int iCol=0;
  int iSt=0;
  int iSpan6=6;
  int iNum=0;
  int iLen=0;
  string buff;
  bool cont =true;
  while(cont){
      buff= src.substr(iSt );
      iLen =buff.length();
      if(iLen >=iSpan6 ){
          buff= src.substr(iSt, iSpan6 );
      }else if(iLen < 1){      
          cont=false;
          return;
      }
      char s16_1[4+1];
      char s16_2[4+1];
      char s16_3[4+1];
      if(buff.length() >= iSpan6){
          string c1=buff.substr(0, 2 );
          string c2=buff.substr(2, 2 );
          string c3=buff.substr(4, 2 );
          sprintf(s16_1, "%s" , c1.c_str() );
          sprintf(s16_2, "%s" , c2.c_str() );
          sprintf(s16_3, "%s" , c3.c_str() );
          unsigned int i16_1= hexToUInt(s16_1);
          unsigned int i16_2= hexToUInt(s16_2);
          unsigned int i16_3= hexToUInt(s16_3);
          if(i16_1 == 0xFF){
printf("# buildMapCustom= %d\n" ,i16_1);
              buildMapCustom(i16_1, i16_2 , i16_3);
          }else{
              buildMapCMP(i16_1, i16_2 , i16_3);
          }
          nPatternMax++;
//printf("i16_1=%x\n" , i16_1);
      }
      iSt = iSt+iSpan6;
      iCol++;
  }
}

//Util
//
void dump_pattern(int src[8][8]){
  for(int i=0; i<8; i++){
    for(int j=0; j<8; j++){
      printf("%d" ,src[i][j]);
    }
      printf(" \n");
  }
  printf("--------\n");
  printf("\n");
}

void clear_pattern(){
    for(int i=0;i< mMaxStr; i++){
        for(int j=0; j< mLED_COL; j++){
            pat16[i][j]=0;
        }
    }
}

void setPattern_all(int pattern, int iRow, char *line){
    int buff[8][8];
    for(int i=0; i< mLED_COL; i++){
        if(line[i]=='1'){
            buff[iRow][i]=1;
        }else{
            buff[iRow][i]=0;
        }
    }

    for(int row=0; row< mLED_COL; row++){
        for(int col=0; col< mLED_COL; col++){
            patterns[pattern][col][row] = buff[row][7-col];
        }
    }
}

char* intToBin(int src, int iRow, int pattern){
    char *ret;
        int    iLen=mLED_COL;
        char    buff[iLen+1];
        int     bin;
        int     i1;

        bin = src;
        buff[iLen] = '\0';
        for(int j = 0; j < iLen; j++){
            buff[j]='0';
        }
        
        for(i1 = 0; i1 < iLen; i1++){
                if(i1 != 0 && bin == 0)
                        break;
                if(bin % 2 == 0)
                        buff[(iLen-1)-i1] = '0';
                else
                        buff[(iLen-1)-i1] = '1';
                bin = bin / 2;
        }
//printf(">>>%s\n", buff );
        setPattern_all(pattern, iRow, buff);
        
        ret= &buff[0];
   return ret;
}


//
void init_proc(){
    for (int i = 1; i <= 8; i++) {
        mCols[i-1]=0;
    }
    for (int i = 1; i <= 8; i++) {
        mRows[i-1]=0;
    } 
    clear_pattern();  
}
//
void display_proc(int pattern){
    int leds[8][8];
    //first-char
    for (int i = 0; i < mLED_COL; i++) {
      for (int j = 0; j < mLED_COL; j++) {
           leds[i][j] = patterns[0][i][j];
      }
    }  
    for(int k=0; k < mLED_COL; k++){
        for(int i=0; i < 10; i++){     // Loop-LED-ON
            for(int col=0; col<8 ; col++){
                mRows[col]=0;
                for(int row=0; row<8; row++){
                    if(leds[col][row]==1){
                        mCols[row]=1;
                    }
                    wait_us(200);
                    if(leds[col][row]==1){
                        mCols[row]=0;
                    }
                    mCols[row]=0;
                }
                mRows[col]=1;
            }
        }
        //slide
//mPc.printf("pattern=%d\n" ,pattern);
        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
              if(j ==7){
 // mPc.printf("dat-7=%d\n" ,patterns[nPattern +1][i][iCt]);
               leds[i][7] = patterns[1][i][k];
              }else{
               leds[i][j] = leds[i][j+1];
              }
          }
        }
    } //for_k
// mPc.printf("display_proc.nPattern=%d ,mSTAT=%d, k=%d\n", nPattern ,mSTAT, k);
}

//
bool Is_validLine(string src){
    int ret=false;
    int nMax=32;
    int iLen= strlen(src.c_str() );
    string sRes32="";
    if(iLen > nMax){
      sRes32= src.substr(0, nMax);
    }else{
      sRes32=src;
    }
    char sLine[nMax];
//    int iLen= strlen(sLine);
    if(iLen>= 2){
        sprintf(sLine, "%s" ,sRes32.c_str());
//printf("valid.iLen=%d\n" , iLen );
//printf("x=%02X\n", sLine[iLen-2] );   
//printf("x=%02X\n", sLine[iLen-1] ); 
        if(sLine[iLen-2]=='\r'){
            if(sLine[iLen-1]=='\n'){
//    printf("[sE2].x=%02X\n", sLine[iLen-2] );   
//    printf("[sE1].x=%02X\n", sLine[iLen-1] ); 
                ret=true;
            }                        
        }
    }
    return ret;
}

//
void  set_nextString(const char *ss){
  int i=0;
  while(ss[i] != '\0'){
//    sprintf(buff, "%02X", ss[i]);
    if(!((ss[i]=='\r') || (ss[i]=='\n'))){
        mReceive2+=ss[i];
    }
    i++;
  }
//printf("mReceive2=%s\n" , mReceive2.c_str() );  
}


// 
void proc_uart(){
    if( mPc.readable()) {
        char c= mPc.getc();
        mResponse+= c;
    }else{
                if(mSTAT ==mSTAT_RSV_START){
                  if(mResponse.length() > 5){
//printf("mResponse=%s,resLen%d\n" ,mResponse.c_str() ,mResponse.length() );
                    string sStat=mResponse.substr(2,1);
//printf("sStat=%s\n" ,sStat.c_str() );
                    if(sStat== "1"){
                        string sNo=mResponse.substr(0,2);
//printf("sNo=%s\n" , sNo.c_str() );
                        iMC = atoi( sNo.c_str() );
                        mHtypRecv=mResponse.substr(3,2);
//                        mReceive =mResponse.substr(3 );
                        mReceive =mResponse.substr(5 );
//printf("mReceive=%s\n" ,mReceive.c_str() );
                        if(iMC <2){
                              string strSP=string(mSpace);
                              mReceive =strSP+ mReceive;
                        }
printf("mReceive=%s\n" ,mReceive.c_str() );
printf("mHtypRecv=%s\n" ,mHtypRecv.c_str() );
                        if(Is_validLine(mResponse)==true ){
                            //if(iMC <2){
                            //  mReceive+=" ";
                            //}                            
                            mSTAT =mSTAT_DISP;
                        }else{
                            mSTAT =mSTAT_RSV_END;
                        }
                        mResponse="";
                    }else{
                        mResponse="";                        
                    }
                  }
                }else if(mSTAT ==mSTAT_RSV_END){
                  if(mResponse.length() > 0){
                        mReceive += mResponse;
//printf("[b]mResponse=%s,resLen%d\n" ,mResponse.c_str() ,mResponse.length() );
printf("[b]mReceive=%s\n" ,mReceive.c_str()  );
                        if(Is_validLine(mResponse)==true ){
                            mSTAT =mSTAT_DISP;
                        }
                        mResponse="";
                  }
                } //if_mStat

    }// if_read                
}

//
void display_custom(){
    int leds[8][8];
    //first-char
    for (int i = 0; i < mLED_COL; i++) {
      for (int j = 0; j < mLED_COL; j++) {
           leds[i][j] = patterns[0][i][j];
      }
    }  
    for(int k=0; k < mLED_COL; k++){
        for(int i=0; i < 10; i++){     // Loop-LED-ON
            for(int col=0; col<8 ; col++){
                mRows[col]=0;
                for(int row=0; row<8; row++){
                    if(leds[col][row]==1){
                        mCols[row]=1;
                    }
                    wait_us(200);
                    if(leds[col][row]==1){
                        mCols[row]=0;
                    }
                    mCols[row]=0;
                }
                mRows[col]=1;
            }
        }
    } //for_k
// mPc.printf("display_proc.nPattern=%d ,mSTAT=%d, k=%d\n", nPattern ,mSTAT, k);
}

//
// sHtyp: 01=One disp ,02=Scrool
void proc_mainLoop(string sHtyp){
    int iTyp= atoi(sHtyp.c_str() );
    if( iTyp==mHtyp_One){
      for(int i=0; i< 2; i++){
            for(int k=0; k < nPatternMax; k++){
                for(int iPos=0; iPos < 2; iPos++){
                    for(int row=0;row < mLED_COL; row++ ){
                        char *bit= intToBin(pat16[k+iPos][row], row, iPos);
                    }
                }
            }
            display_custom();
        wait_ms(100);
      }
    }else{
        for(int k=0; k < nPatternMax; k++){
        //mPc.printf("main.k=%d\n", k );
            for(int iPos=0; iPos < 2; iPos++){
                for(int row=0;row < mLED_COL; row++ ){
                    char *bit= intToBin(pat16[k+iPos][row], row, iPos);
                }
            }
    //debug
    //for(int iPos=0; iPos < 2; iPos++){
    //     dump_pattern(patterns[iPos]);
    //}
    //mPc.printf("main.k=%d\n", k );
          display_proc(k);
        }//for_k
    }
}


//
int main() {
    mSTAT = mSTAT_RSV_START;
    mPc.baud(9600 );
    mPc.printf("#Start-main\n");
    init_proc();
    while(1){
        wait_ms(5);
//wait_ms(100);
//printf("mSTAT=%d\n", mSTAT);
        if(mSTAT != mSTAT_DISP){
            proc_uart();
        }else{            
            set_nextString(mReceive.c_str());
printf("mReceive2.length()=%d\n", mReceive2.length());
printf("mReceive2 =%s\n", mReceive2.c_str() );
            drawHextext( (char *)mReceive2.c_str() );
printf("nPatternMax=%d\n" ,nPatternMax);
            proc_mainLoop(mHtypRecv);
            mSTAT = mSTAT_RSV_START;
            clear_pattern();
//printf("#End-dispkay.len=%d \n", mReceive2.length() );
 printf("#End-dispkay\n"  );
            mReceive="";
            mReceive2="";
            nPatternMax=0;    
//debug
/*
for(int i=0; i< nPatternMax ;i++){
    for(int j=0; j<mLED_COL; j++){
        printf("%02x ",pat16[i][j]);
    }
    printf("\n");
}            
*/
        }
    } //end_while
    printf("#End-main \n");
}