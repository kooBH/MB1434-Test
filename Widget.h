#ifndef _H_WIDGET_
#define _H_WIDGET_

#include "QComboBox"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextBrowser>

#include <QString>

#include <thread>

#include <iostream>
#include "serial/serial.h"

#include <vector>
#include <map>
using std::vector;
using std::string;
using std::to_string;
using std::map;
using std::make_pair;

class Sonar : public QWidget {
  private :
    /*
     * -----------------
     * | port   | out- | 
     * | btn    | put  |
     * | btn    |      |
     * -----------------
     * */
    QHBoxLayout layout_main;

    QWidget widget_left;
    QVBoxLayout layout_left;
    QComboBox combo_serial;
    QPushButton btn_search;
    QPushButton btn_run;

    QTextBrowser TB_device;

    QString text_device;	
    map<string,unsigned int> map_serial_device;

    std::thread *thread_run;
    bool is_runnig;

    //https://stackoverflow.com/questions/12417210/how-to-convert-unsigned-char-to-qstring
    QString GetQStringFromUnsignedChar( unsigned char *str ){
      QString result = "";
      int lengthOfString = strlen( reinterpret_cast<const char*>(str) );

      // print string in reverse order
      QString s;
      for( int i = 0; i < lengthOfString; i++ ){
        s = QString( "%1" ).arg( str[i], 0, 16 );

        // account for single-digit hex values (always must serialize as two digits)
        if( s.length() == 1 )
          result.append( "0" );

        result.append( s );
      }

      return result;
    }

  public : 
    inline Sonar();
    inline ~Sonar();
    inline void Serialprobe();

    inline void Reading();
    inline void _run();

};

inline Sonar::Sonar():
btn_search("SEARCH"),
btn_run("RUN"),
thread_run(nullptr),
is_runnig(false)
{

  layout_left.addWidget(&combo_serial);
  layout_left.addWidget(&btn_search);
  layout_left.addWidget(&btn_run);

  widget_left.setLayout(&layout_left);
   
  layout_main.addWidget(&widget_left);
  layout_main.addWidget(&TB_device);
  setLayout(&layout_main);

  QObject::connect(&btn_search,&QPushButton::clicked,[&](){Serialprobe();});
  QObject::connect(&btn_run,&QPushButton::clicked,[&](){Reading();});

}
inline Sonar::~Sonar(){

}

inline void Sonar::Serialprobe(){  
  text_device = " *** Device List *** \n";

  vector<serial::PortInfo> devices_found = serial::list_ports();

  vector<serial::PortInfo>::iterator iter = devices_found.begin();
  //printf("Found %lu Devices\n", devices_found.size());
  //printf("=======================\n");
  map_serial_device.clear();
  int i = 0;
  while (iter != devices_found.end()) {
    serial::PortInfo device = *iter++;
    if (strcmp("n/a", device.hardware_id.c_str())){
      map_serial_device.insert(make_pair(device.port.c_str(), i));
      i++;
      text_device.append("\nPort = ");
      text_device.append(QString::fromStdString(device.port.c_str())); 
      text_device.append("\ndescription = ");
      text_device.append(QString::fromStdString(device.description.c_str())); 
      text_device.append("\nhardware ID = ");
      text_device.append(QString::fromStdString(device.hardware_id.c_str()));
      text_device.append("\n"); 
    }
  }
  combo_serial.clear();
  for(auto it = map_serial_device.begin();it !=map_serial_device.end();it++){
    combo_serial.addItem( QString::fromStdString(it->first));
  }
  TB_device.setText(text_device);
}

inline void Sonar::Reading(){

  if(!is_runnig){
  is_runnig=true;

  if(thread_run)
    delete thread_run;
  printf("run thread\n");
  thread_run = new std::thread(&Sonar::_run,this);
  thread_run->detach();
  }else{
  
     is_runnig = false;
  
  }

}

inline void Sonar::_run(){

  unsigned long baud = 57600;
  string port = combo_serial.currentText().toStdString();

  serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000)); 

  while(is_runnig){
    try{
      //  cout << "Is the serial port open?";
      if (my_serial.isOpen())
      {

        uint8_t data_read[8];
        size_t available;
        memset(data_read, 0, sizeof(uint8_t) * 8);
        available = my_serial.available();
        size_t string_read = my_serial.read(data_read, 8);
        printf("%c",data_read[0]);
        printf("%c",data_read[1]);
        printf("%c",data_read[2]);
        printf("%c",data_read[3]);
        printf("%c",data_read[4]);
        printf("%c",data_read[5]);
        printf("%c",data_read[6]);
        printf("%c",data_read[7]);
        printf("\n");
        //TB_device.setText(GetQStringFromUnsignedChar(data_read));
        
      }
    }
    catch (std::exception const& e){
      //TB_device.setText("error! port is not found\n");
      printf("error! port is not found\n");
      
      break;
    }
  }

}

#endif
