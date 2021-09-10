#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

class StateMachine{

    public:
        const static int ST_BEGIN = 0;
        const static int ST_END = 1;
        const static int ST_ERROR = 2;
        int currentState;

    public:
        StateMachine();
        ~StateMachine();

        void setCurrentState(int currentState);

        int getCurrentState();

        bool begin();
        bool end();
        bool error();
};

#endif // STATEMACHINE_HPP
