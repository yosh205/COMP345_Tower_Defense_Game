/**
 * @file MapObserver.h
 * @brief Declares MapObserver, a view that redraws a Map every time it changes.
 */
#ifndef MapObserver_h
#define MapObserver_h

#include "../Map/Map.h"
#include "../Observer/Observer.h"
#include "../GUI/MapView.h"
#include <SFML/Graphics.hpp>

/** 
 * @brief Observer that displays a Map in a window and redraws it on every change.
 * Observer pattern
 */
class MapObserver : public Observer {
    public:
        /**
         * @brief Attaches to map and draws it once.
         * @param map The map to observe.
         * @param window The window to draw the map in.
         */
        MapObserver(Map& map, sf::RenderWindow& window);

        /** @brief Detached from the map. */
        ~MapObserver() override;

        /** @brief Not copyable: the map stores this observer's address */
        MapObserver(const MapObserver&) = delete;
        /** @brief Not copyable: the map stores this observer's address */
        MapObserver& operator=(const MapObserver&) = delete;

        /**
         * @brief Called by the map when it changes: logs the update and redraws
         * @param subject The map that changed
         */
        void update(Observable* subject) override;

        /** @brief Draws the map and its towers, scaled to fit the window. */
        void render();

        /** @return How many times update() has been called. */
        int getUpdateCount() const;

    private:
        Map* map; ///< The map to observe (not owned).
        sf::RenderWindow* window; ///< The window to draw the map in (not owned).
        MapView view; ///< Draws the map in the window.
        int updateCount = 0; ///< How many times update() has been called.
};


#endif // MapObserver_h