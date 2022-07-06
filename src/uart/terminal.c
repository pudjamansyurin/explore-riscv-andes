/*
 * terminal.c
 *
 *  Created on: May 5, 2022
 *      Author: pujak
 */
#include "terminal.h"
#include "gui-driver/gui_cmd.h"
#include "tinysh/tinysh.h"
#include <stdio.h>

/* Private variables */
static char buffer[256];

/* Private function definitions */
static void guicmd_init(void)
{
	static tinysh_cmd_t gui_cmds[GUI_CMD_CNT];
	static uint8_t u8_mdlBuffer[500*1024];
	uint8_t u8_i;

	vMdl_init(u8_mdlBuffer, sizeof(u8_mdlBuffer), NULL);

	/* initialize GUI commands */
	for(u8_i=0; u8_i<GUI_CMD_CNT; u8_i++)
	{
		const guicmd_t* p_gui = vGuiCmd_get(u8_i);
		tinysh_cmd_t* p_tsh = &(gui_cmds[u8_i]);

		p_tsh->name = p_gui->name;
		p_tsh->help = p_gui->help;
		p_tsh->usage = p_gui->usage;
		p_tsh->function = p_gui->function;

		tinysh_add_command(p_tsh);
	}
}

/* Public function definitions */
void terminal_init(NDS_DRIVER_USART *p_usart, uint32_t u32_baud,
		stdin_reader_t reader, stdout_locker_t locker)
{
	/* setup serial */
	serial_init(p_usart, u32_baud, locker);
	serial_start(reader, buffer, sizeof(buffer));

	/* setup tiny-shell */
	tinysh_set_prompt("\r\n\nVT> ");

	/* initialize GUI driver */
	guicmd_init();
}

void terminal_in(unsigned char *str, uint16_t size)
{
	while (*str && size--)
	{
		tinysh_char_in(*str++);
	}
}

/* we must provide this function to use tinysh  */
void tinysh_puts(char *s)
{
	printf("%s", s);
}

void tinysh_putchar(unsigned char c)
{
	putchar((int) c);
}

