/**
 * @file TowerObserver.h
 * @brief Observer that redisplays a tower whenever that tower changes.
 */
#ifndef TowerObserver_h
#define TowerObserver_h

#include <iosfwd>

#include "../Observer/Observer.h"

class Observable;

/**
 * @brief Displays a tower's view when the tower notifies it.
 *
 * Plug one into a tower with plugInto(). Each time the tower changes a value
 * (for example upgrade()), Observable::notify() calls update(), which prints
 * the tower's type, cell, level, range, power, fire rate and refund.
 * The observer does not own the tower. It detaches itself when destroyed.
 */
class TowerObserver : public Observer {
public:
    /**
     * @brief Creates an observer that writes the tower view to out.
     * @param out Stream that receives the view (for example std::cout).
     */
    explicit TowerObserver(std::ostream& out);

    /** @brief Stops watching, if this observer was plugged into a tower. */
    ~TowerObserver() override;

    TowerObserver(const TowerObserver&) = delete;
    TowerObserver& operator=(const TowerObserver&) = delete;

    /**
     * @brief Attaches this observer to a tower (or any Observable).
     * If it was already watching something else, it detaches from that first.
     * @param subject The tower to watch.
     */
    void plugInto(Observable& subject);

    /**
     * @brief Redraws the tower view. Called by the tower after a change.
     * @param subject The tower that changed. Ignored if it is not a Tower.
     */
    void update(Observable* subject) override;

private:
    std::ostream& out;              ///< Where the view is written.
    Observable* subject = nullptr;  ///< Tower being watched, or nullptr.
};

#endif // TowerObserver_h
