/**
 * @file Observer.h
 * @brief Declares Observer, the interface for objects that react when an Observer changes.
 */
#ifndef Observer_h
#define Observer_h

class Observable; // forward declaration

/**
 * @brief Interface for objects that want to be told when an Observable changes.
 */
class Observer {
    public:
        /**
         * @brief Virtual so observers can be deleted through an Observer pointer.
         */
        virtual ~Observer() = default;

        /**
         * @brief Called by the subject every time it changes.
         * @param subject The object that changed.
         */
        virtual void update(Observable* subject) = 0; // pure virtual
    };

#endif // Observer_h