// code related to user interface : led and button

void setupIntf();

// state value : '0' = off , '1' = on , 'B' = blink slowly , 'E' = blick quickly (error state)
void setLed(char state);

// timer callback to blink led
void updateLed(void *, long, int);

// return true if button was clicked
// if button is pressed, function is blocking until it's released
bool click();