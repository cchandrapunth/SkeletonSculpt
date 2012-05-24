#include "stdafx.h"

#include "log.h"


Log::Log(char* filename) {
  m_stream.open(filename);
}

void Log::Write(char* logline){
  m_stream << logline << endl;
}

Log::~Log(){
  m_stream.close();
}