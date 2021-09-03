#include "StateMachine.h"

StateMachine::StateMachine() {

    currentState =-1;
}

StateMachine::~StateMachine(){
}

void StateMachine::setCurrentState(int currentState) {

    this->currentState = currentState;
}

int StateMachine::getCurrentState() {

    return currentState;
}

bool StateMachine::begin(){

    return (getCurrentState() == 0);
}

bool StateMachine::end(){

    return (getCurrentState() == 1);
}

bool StateMachine::error(){

    return (getCurrentState() == 2);
}
