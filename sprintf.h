#ifndef SPRINTF_H
#define SPRINTF_H

#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <wchar.h>

#include "s21_string.h"

#define TOKENS "cdieEfgGosuxXpn"
#define BUFFER_SIZE 1024
#define NOT_DECIMAL_RESERVE 64

char conflicting_char = '.';

typedef struct var_parameters {
  int align_minus;
  int show_sign;
  int space_for_pos;
  int hash_zero;
  int direct_zero;
  int width;
  int star_width;
  int star_precision;
  int precision_zero;
  int dot_check;
  int precision_num;
  int len_h;
  int len_l;
  int length_double;
} flags_t;

void append_string(va_list ap, flags_t *flags, char *buffer,
                   int *current_symbol);

void append_hex(va_list ap, flags_t *flags, long unsigned int v_x, char *buffer,
                int *current_symbol, const char *format);

void append_g(va_list ap, flags_t *flags, long double v_e, char *buffer,
              int *current_symbol, const char *format_pointer);

void append_float(va_list ap, flags_t *flags, long double v_f, char *buffer,
                  int *current_symbol);

void append_exp(va_list ap, flags_t *flags, long double v_e, char *buffer,
                int *current_symbol, const char *format_pointer);

void append_char(va_list ap, flags_t *flags, char *buffer, int *current_symbol);

void append_int(va_list ap, flags_t *flags, char *buffer, int *current_symbol,
                const char *gen_pointer);

void append_octal(va_list ap, flags_t *flags, char *buffer,
                  int *current_symbol);

void append_pointer(va_list ap, flags_t *flags, int *v_p, char *buffer,
                    int *current_symbol);

void buffer_writing(const char *current_ptr, char *buffer, int *current_symbol,
                    int count, int flag, int hex_flag, int sign);

void align_left(char *buffer, int *current_symbol, int len, int fill,
                flags_t *flags, const char *current_ptr);

int add_zero(char **current_ptr, char *buffer, int *current_symbol, int len,
             flags_t *flags, int *shift);

void add_space(const char *current_ptr, char *buffer, int *current_symbol,
               int len, flags_t *flags);

int parse_params(const char *current_ptr, flags_t *flags, int *shift);

void init_struct(flags_t *res);

char *s21_itoa(long int value);

char *oct_to_string(long int value, flags_t *flags);

void delete_zero(char *g_string, int *current_symbol);

char *hex_to_string(unsigned long int value, int upper_flag, int pointer_flag,
                    int nil_flag);

char *float_to_string(long double value, int g_flag, flags_t *flags);

char *exp_to_string(long double v_e, const char *exp_letter, int g_flag,
                    flags_t *flags);

int check_fraction(int *current_symbol, flags_t *flags, long double value);

int check_precision(flags_t *flags, int expo);

int add_precision_zero(char **current_ptr, char *buffer, int *current_symbol,
                       flags_t *flags, int flag_x, int *shift);

void add_space_precision_zero(char *current_ptr, char *buffer,
                              int *current_symbol, flags_t *flags);

int find_exp(long double value);

void long_char_helper(wchar_t *long_str, flags_t *flags, char *s,
                      int *current_symbol);

void null_helper(flags_t *flags, char *s, int *current_symbol);

void normal_string_helper(char *string, flags_t *flags, char *s,
                          int *current_symbol);

#endif  // SPRINTF_H
