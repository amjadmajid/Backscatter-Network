/*
 * node.c
 *
 *  Created on: 21 Jun 2018
 *      Author: amjad
 */

#include "node.h"
static uint8_t node_id = 0;

void set_node_id(uint8_t id)
{
    node_id = id;
}
uint8_t get_node_id()
{
    return node_id;
}

