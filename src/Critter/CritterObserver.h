/**
 * @file CritterObserver.h
 * @brief Observer that redisplays a Critter when it changes.
 */
#ifndef CritterObserver_h
#define CritterObserver_h

#include <iosfwd>

#include "../Observer/Observer.h"

class Observable;

/**
 * @brief Displays a critter's view when the critter notifies it.

 * Plug one into a critter with plugInto(). Each time the critter changes a value
 * (for example takeDamage()), Observable::notify() calls update(), which prints
 * the critter's kind, health, speed, damage and position.
 * The observer does not own the critter. It detaches itself when destroyed.
 */
class CritterObserver : public Observer {
public:
    /**
     * @brief Creates an observer that writes the critter view to out.
     * @param out Stream that receives the view (for example std::cout).
     */
    explicit CritterObserver(std::ostream& out);

    /** @brief Stops watching, if this observer was plugged into a critter. */
    ~CritterObserver() override;

    CritterObserver(const CritterObserver&) = delete;
    CritterObserver& operator = (const CritterObserver&) = delete;

    /**
     * @brief attaches this observer to a critter (or any Observable).
     * If it was already watching something else, it detaches from that first.
     * @param subject The critter to watch.
     */
    void plugInto(Observable& subject);

    /**
     * @brief Redraws the critter view. Called by the critter after a change.
     * @param subject The critter that changed. Ignored if it is not a Critter.
     */
    void update(Observable* subject) override;

private:
    std::ostream& out;                  ///< Where the view is written.
    Observable* subject = nullptr;      ///< Critter being watched, or nullptr.
};

#endif // CritterObserver_h