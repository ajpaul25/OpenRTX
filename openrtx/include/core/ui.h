/***************************************************************************
 *   Copyright (C) 2020 - 2023 by Federico Amedeo Izzo IU2NUO,             *
 *                                Niccolò Izzo IU2KIN,                     *
 *                                Silvano Seva IU2KWO                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * This function initialises the User Interface, starting the
 * Finite State Machine describing the user interaction.
 */
void ui_init();

/**
 * This function writes the OpenRTX splash screen image into the framebuffer.
 *
 * @param centered: if true the logo will be printed at the center of
 * the screen, otherwise it will be printed at the top of the screen.
 */
void ui_drawSplashScreen(bool centered);

/**
 * This function updates the local copy of the radio state
 * the local copy is called last_state
 * and is accessible from all the UI code as extern variable.
 */
void ui_saveState();

/**
 * This function advances the User Interface FSM, basing on the
 * current radio state and the keys pressed.
 *
 * @param sync_rtx: If true RTX needs to be synchronized
 */
void ui_updateFSM(bool *sync_rtx);

/**
 * This function redraws the GUI based on the last radio state.
 *
 * @return true if GUI has been updated and a screen render is necessary.
 */
bool ui_updateGUI();

/**
 * Push an event to the UI event queue.
 *
 * @param type: event type.
 * @param data: event data.
 * @return true on success false on failure.
 */
bool ui_pushEvent(const uint8_t type, const uint32_t data);

/*
 * This function registers a new entry in the info menu, the entry
 * takes the form of a key-value pair.
 * @param key: the key is a static string and cannot be changed
 * @param value_cb: this callback is called to generate the value, Which
 * should be re-generated everytime the info menu is rendered.
 */
void ui_registerInfoExtraEntry(const char *key, char *(*value_cb)());

/**
 * This function terminates the User Interface.
 */
void ui_terminate();

#ifdef __cplusplus
}
#endif

#endif /* UI_H */
