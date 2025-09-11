#include <stdio.h> // stdout, FILE

#include "unity.h" // RUN_TEST()
#include "si_terminfo.h" // si_terminfo_t
#include "si_pbar.h" // si_pbar_t

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

static void si_pbar_test_main(void)
{
	si_terminfo_t terminal_info = {0};
	si_pbar_t progress_bar = {0};
	si_true_color_t label_color = {0};
	si_true_color_t start_color = {0};
	si_true_color_t stop_color = {0};

	si_terminfo_init(&terminal_info, stdout);
	si_pbar_init(&progress_bar);
	progress_bar.alignment = PBAR_ALIGN_BOTTOM;
	si_true_color_from_basic(&label_color, ANSI_COLOR_WHITE, true);
	si_true_color_from_basic(&start_color, ANSI_COLOR_RED, false);
	si_true_color_from_basic(&stop_color, ANSI_COLOR_GREEN, true);
	progress_bar.p_label_color = &label_color;
	progress_bar.p_begin_color = &start_color;
	progress_bar.p_end_color = &stop_color;

	si_pbar_terminal_setup(&progress_bar, &terminal_info);
	double fake_progress = 0.0;
	const double fake_step = 0.038;
	while(1.0 >= fake_progress)
	{
		fake_progress += fake_step;
		printf("Example output before updated.\n");
		si_pbar_fprint(&progress_bar, &terminal_info, fake_progress);
		sleep(1);
		printf("Example output after updated.\n");
	}
	si_pbar_terminal_restore(&progress_bar, &terminal_info);
}

static void si_pbar_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_pbar_test_main);
	UNITY_END();
}

int main(void)
{
	si_pbar_test_all();
	return 0;
}
