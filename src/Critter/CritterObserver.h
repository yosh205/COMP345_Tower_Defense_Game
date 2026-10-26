/**
 * @file CritterObserver.h
 * @brief Observer that redisplays critters whenever one of them changes.
 */
#ifndef CritterObserver_h
#define CritterObserver_h

#include <iosfwd>
#include <vector>

#include <SFML/Graphics.hpp>

#include "../GUI/CritterView.h"
#include "../GUI/MapView.h"
#include "../Map/Map.h"
#include "../Observer/Observer.h"
#include "Critter.h"

class Observable;

/**
 * @brief Displays the critter view when a watched critter notifies it.
 *
 * Plug critters in with plugInto() and remove them with unplug(). Each add or
 * remove redraws the view. Afterwards, every change on a watched critter
 * (damage, slow, position) calls Observable::notify(), which calls update()
 * and redraws again. The view is printed to a stream and, once showOn() has
 * been called, drawn on that window over the map.
 * The observer does not own the critters. It detaches itself when destroyed,
 * so the critters must still be alive at that moment.
 */
class CritterObserver : public Observer {
public:
    /**
     * @brief Creates an observer that writes the critter view to out.
     * @param out Stream that receives the view (for example std::cout).
     */
    explicit CritterObserver(std::ostream& out);

    /** @brief Stops watching every critter this observer was plugged into. */
    ~CritterObserver() override;

    CritterObserver(const CritterObserver&) = delete;
    CritterObserver& operator=(const CritterObserver&) = delete;

    /**
     * @brief Draws the watched critters in window, on top of map.
     * Does not attach to the map: the map is only the background.
     * @param window The window to draw into.
     * @param map    The map the critters are standing on.
     */
    void showOn(sf::RenderWindow& window, const Map& map);

    /**
     * @brief Starts watching critter and redisplays the view.
     * Attaching the same critter twice has no effect.
     * @param critter The critter to watch.
     */
    void plugInto(Critter& critter);

    /**
     * @brief Stops watching critter and redisplays the view.
     * Does nothing if this observer was not watching it.
     * @param critter The critter to stop watching.
     */
    void unplug(Critter& critter);

    /**
     * @brief Redraws the critter view. Called by a critter after a change.
     * @param subject The critter that changed. Ignored if it is not one of the watched critters.
     */
    void update(Observable* subject) override;

    /** @brief Draws the map and every watched critter. Does nothing until showOn(). */
    void render();

    /** @return How many critters are currently plugged in. */
    int getWatchedCount() const;

private:
    /** @brief Writes one critter's kind, health, speed and position to out. */
    void printView(const Critter& critter, const char* event);

    std::ostream& out;                       ///< Where the text view is written.
    std::vector<Critter*> observedCritters;  ///< Critters being watched (not owned).
    sf::RenderWindow* window = nullptr;      ///< Window from showOn(), or nullptr.
    const Map* map = nullptr;                ///< Map from showOn(), or nullptr.
    MapView mapView{40.f};                   ///< Draws the map behind the critters.
    CritterView critterView;                 ///< Draws each watched critter.
};

#endif // CritterObserver_h
