void setupMotors();

unsigned long prepareMove(long len);
unsigned long prepareTurn(long angle);
unsigned long prepareArc(long radius, int angle);

void penUp();
void penDown();

void doSubStep(char command, unsigned long substep);