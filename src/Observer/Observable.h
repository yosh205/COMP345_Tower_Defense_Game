/**
 * @file Observable.h
 * @brief Declares Observable, the base class for objects that observers can watch.
 */
#ifndef Observable_h
#define Observable_h

#include <vector>
#include "Observer.h"

/**
 * @brief Base class for objects that notify observers when they change.
 * 
 * The subclasses (Tower, Map, Critter) calls notify at the end of every
 * function that changes its state. Each attached Observer then gets update().
 */
class Observable {
    public:
        /** @brief Creates a subject with no observers. */
        Observable() = default;

        /** @brief Copies nothing: the copy starts with no observers */
        Observable(const Observable& other);

        /**
         * @brief Assignment keeps this object's own observers; the other's are not copied.
         * @return This object.
         */
        Observable& operator=(const Observable& other);

        /**
         * @brief Starts notifying observer about changes. Attaching twice has no effect.
         * @param observer The observer to add (must not be full).
         */
        void attach(Observer* observer);

        /**
         * @brief Stops notifying observer. Does nothing if it is not attached.
         * @param observer The observer to remove.
         */
        void detach(Observer* observer);

        /** @return Number of attached observers. */
        int getObserverCount() const;

    protected:
        /**
         * @brief Calls update() on every attached observer.
         * Subcalsses call this after each change to their state.
         */
        void notify();

    private:
        std::vector<Observer*> observers; ///< Observers to notify (not owned).
};

#endif // Observable_h
