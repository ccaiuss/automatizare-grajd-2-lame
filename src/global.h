
void status(boolean val)
{
  digitalWrite(PIN_STATUS, val);
}
void resetare_cronometru(){
count_sec = 0;
sec = nrOre * SECS_PER_HOUR;
}
void stop_motoare()
{
  Serial.println("MOTOARE OPRITE");
  digitalWrite(PIN_M_JOS, OFF);
  digitalWrite(PIN_M_SUS, OFF);
}
void motor_on(String sens)
{
  
  if (sens == "sus")
  {
    digitalWrite(PIN_M_JOS, OFF);
    if (timer_m_sus > 500)
    {
      digitalWrite(PIN_M_SUS, ON);
    }
    timer_m_jos = 0;
  }
  else if (sens == "jos")
  {
    digitalWrite(PIN_M_SUS, OFF);
    if (timer_m_jos > 500)
    {
      digitalWrite(PIN_M_JOS, ON);
    }
    timer_m_sus = 0;
  }
}
void motoare_off()
{
  timer_m_jos = 0;
  timer_m_sus = 0;
  digitalWrite(PIN_M_SUS, OFF);
  digitalWrite(PIN_M_JOS, OFF);
}
