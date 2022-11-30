#include "sprintf.h"

int s21_sprintf(char *buffer, const char *format, ...) {
  int current_symbol = 0;
  const char *current_ptr = S21_NULL;
  int stop_flag = 0;
  long double v_e = 0;
  long double v_f = 0;
  int *v_p = S21_NULL;
  long unsigned int v_x = 0;
  flags_t flags;

  va_list ap;
  va_start(ap, format);
  setlocale(LC_ALL, "");
  setlocale(LC_NUMERIC, "en_US.UTF-8");
  for (current_ptr = format; *current_ptr; current_ptr++) {
    int shift = 0;
    if ((*current_ptr) != '%') {
      buffer[current_symbol] = *current_ptr;
      current_symbol++;
      continue;
    } else {
      shift = 0;
      init_struct(&flags);
      stop_flag = parse_params(current_ptr, &flags, &shift);
    }
    if (!stop_flag) {
      current_ptr += shift;
      if (s21_strchr("diu", (int)(*current_ptr))) {
        append_int(ap, &flags, buffer, &current_symbol, current_ptr);
      } else if (*current_ptr == 'c') {
        append_char(ap, &flags, buffer, &current_symbol);
      } else if (*current_ptr == 'E' || *current_ptr == 'e') {
        append_exp(ap, &flags, v_e, buffer, &current_symbol, current_ptr);
      } else if (*current_ptr == 'f') {
        append_float(ap, &flags, v_f, buffer, &current_symbol);
      } else if (*current_ptr == 'G' || *current_ptr == 'g') {
        append_g(ap, &flags, v_e, buffer, &current_symbol, current_ptr);
      } else if (*current_ptr == 'o') {
        append_octal(ap, &flags, buffer, &current_symbol);
      } else if (*current_ptr == 's') {
        append_string(ap, &flags, buffer, &current_symbol);
      } else if (*current_ptr == 'x' || *current_ptr == 'X') {
        append_hex(ap, &flags, v_x, buffer, &current_symbol, current_ptr);
      } else if (*current_ptr == 'p') {
        flags.hash_zero = 1;
        append_pointer(ap, &flags, v_p, buffer, &current_symbol);
      } else {
        buffer[current_symbol] = *current_ptr;
        current_symbol++;
      }
    }
  }
  buffer[current_symbol] = '\0';
  va_end(ap);
  return s21_strlen(buffer);
}

void append_pointer(va_list ap, flags_t *flags, int *va, char *array,
                    int *current_symbol) {
  int zero_flag = 0;
  if (flags->star_width == 1) {
    flags->width = va_arg(ap, int);
  }
  va = va_arg(ap, int *);
  int not_null = 0;
  long long int convert = 0;
  if (va != S21_NULL) {
    convert = (long long int)va;
  } else {
    not_null = 1;
  }
  char *ptr_value = hex_to_string(convert, 0, flags->hash_zero, not_null);
  if (flags->width > 0 && flags->align_minus == 0) {
    add_space(ptr_value, array, current_symbol, (int)s21_strlen(ptr_value),
              flags);
  }
  buffer_writing(ptr_value, array, current_symbol, (int)s21_strlen(ptr_value),
                 zero_flag, 0, 0);
  if (flags->align_minus) {
    align_left(array, current_symbol, (int)s21_strlen(ptr_value), flags->width,
               flags, ptr_value);
  }
  free(ptr_value);
}

void append_octal(va_list ap, flags_t *flags, char *array,
                  int *current_symbol) {
  int zero_flag = 0;
  long int v_int;
  if (flags->star_width == 1) {
    flags->width = va_arg(ap, int);
  }
  if (flags->star_precision) {
    flags->precision_num = va_arg(ap, int);
  }
  if (flags->precision_num == -1) flags->precision_num = 0;
  if (flags->len_h == 1) {
    v_int = va_arg(ap, int);
  } else if (flags->len_l == 1) {
    v_int = va_arg(ap, long int);
  } else {
    v_int = va_arg(ap, int);
  }
  v_int = (long int)v_int;
  if (flags->precision_num > 0) {
    char *temp_ar = oct_to_string(v_int, flags);
    int size_helper = (flags->hash_zero > 0) ? (s21_strlen(temp_ar) - 1)
                                             : s21_strlen(temp_ar);
    flags->precision_zero = (flags->precision_num - size_helper > 0)
                                ? (flags->precision_num - size_helper)
                                : 0;
    free(temp_ar);
  }
  int temp_shift = 0;
  char *temp = oct_to_string(v_int, flags);
  if (flags->precision_zero) {
    if (flags->width > 0 && flags->align_minus == 0) {
      add_space_precision_zero(temp, array, current_symbol, flags);
    }
    zero_flag =
        add_precision_zero(&temp, array, current_symbol, flags, 0, &temp_shift);
  } else if (flags->direct_zero) {
    zero_flag = add_zero(&temp, array, current_symbol, (int)s21_strlen(temp),
                         flags, &temp_shift);
  } else if (flags->width > 0 && flags->align_minus == 0) {
    add_space(temp, array, current_symbol, (int)s21_strlen(temp), flags);
  }
  buffer_writing(temp, array, current_symbol, (int)s21_strlen(temp), zero_flag,
                 0, 0);
  if (flags->align_minus) {
    align_left(array, current_symbol, (int)s21_strlen(temp), flags->width,
               flags, temp);
  }
  free(temp - temp_shift);
}

char *oct_to_string(long int num, flags_t *flags) {
  char *res = malloc(sizeof(char) * NOT_DECIMAL_RESERVE);

  if (!res) exit(0);

  char *reverse = malloc(sizeof(char) * NOT_DECIMAL_RESERVE);
  if (!reverse) {
    free(res);
    exit(0);
  }

  int count = 0;
  int sign = (num >= 0) ? 0 : 1;
  if (num == 0) {
    reverse[count] = '0';
    count++;
  }

  if (!sign) {
    int i = 0;
    long int next = 1;
    long int octanum = 0;

    while (num != 0) {
      octanum = octanum + (num % 8) * next;
      num = num / 8;
      next = next * 10;
    }

    while (octanum > 0) {
      next = octanum % 10;
      reverse[count] = next + '0';
      count++;
      octanum /= 10;
    }

    if (flags->hash_zero) {
      reverse[count] = '0';
      count++;
    }

    for (; i < count; i++) {
      res[i] = reverse[count - i - 1];
    }

    res[i] = '\0';
  } else {
    res[0] = '\0';
  }
  free(reverse);
  return res;
}

void append_string(va_list ap, flags_t *flags, char *buffer,
                   int *current_symbol) {
  char *simple_str = S21_NULL;
  wchar_t *long_str = S21_NULL;

  if (flags->star_width) {
    flags->width = va_arg(ap, int);
  }
  if (flags->star_precision) {
    flags->precision_num = va_arg(ap, int);
  }
  if (flags->len_l == 1) {
    long_str = va_arg(ap, wchar_t *);
    if (!long_str) {
      flags->len_l = 0;
    }
  } else {
    simple_str = va_arg(ap, char *);
  }

  if (flags->len_l == 1) {
    long_char_helper(long_str, flags, buffer, current_symbol);
  } else {
    if (!simple_str) {
      null_helper(flags, buffer, current_symbol);
    } else {
      normal_string_helper(simple_str, flags, buffer, current_symbol);
    }
  }
}

void normal_string_helper(char *simple_str, flags_t *flags, char *buffer,
                          int *current_symbol) {
  char reserve[BUFFER_SIZE] = {'\0'};

  s21_strcpy(reserve, simple_str);
  int length = (int)s21_strlen(reserve);

  if ((flags->precision_num > 0) && (flags->precision_num < length)) {
    reserve[flags->precision_num] = '\0';
    flags->width -= flags->precision_num;
  } else if (flags->precision_num != 0) {
    flags->width -= length;
  }

  if ((flags->width > 0) && (flags->align_minus == 0)) {
    for (int k = 0; k < flags->width; k++) {
      buffer[*current_symbol] = ' ';
      *current_symbol = (*current_symbol) + 1;
    }
  }

  if (flags->precision_num != 0) {
    for (int i = 0; i < (int)s21_strlen(reserve); i++) {
      buffer[*current_symbol] = reserve[i];
      *current_symbol = (*current_symbol) + 1;
    }
  }

  if ((flags->width > 0) && (flags->align_minus == 1)) {
    for (int m = 0; m < flags->width; m++) {
      buffer[*current_symbol] = ' ';
      *current_symbol = (*current_symbol) + 1;
    }
  }

  buffer[*current_symbol] = '\0';
}

void long_char_helper(wchar_t *long_str, flags_t *flags, char *buffer,
                      int *current_symbol) {
  if (flags->precision_num == -1) flags->precision_num = 0;

  int length = wcslen(long_str);
  int bytes_helper = sizeof(long_str[0]);
  int size_helper =
      wcstombs(buffer + (*current_symbol), long_str, length * bytes_helper);

  if (flags->precision_num) {
    if (flags->align_minus == 0 && flags->width > size_helper) {
      int space_var = flags->width - flags->precision_num;
      if (space_var > 0) {
        for (int i = 0; i < space_var; i++) {
          buffer[*current_symbol] = ' ';
          *current_symbol += 1;
        }
      }
    }
  } else {
    if (flags->align_minus == 0 && flags->width > size_helper) {
      for (int i = 0; i < (flags->width - size_helper); i++) {
        buffer[*current_symbol] = ' ';
        *current_symbol += 1;
      }
    }
  }

  if (flags->precision_num) {
    size_helper = (flags->precision_num < size_helper) ? flags->precision_num
                                                       : size_helper;
  }
  size_helper = wcstombs(buffer + (*current_symbol), long_str, size_helper);
  *current_symbol += size_helper;
  if (flags->align_minus == 1 && flags->width > size_helper) {
    if (flags->precision_num > size_helper) {
      int space = flags->width - flags->precision_num;
      if (space > 0) {
        for (int i = 0; i < space; i++) {
          buffer[*current_symbol] = ' ';
          *current_symbol += 1;
        }
      }
    } else {
      for (int i = 0; i < (flags->width - size_helper); i++) {
        buffer[*current_symbol] = ' ';
        *current_symbol += 1;
      }
    }
  }
}

void null_helper(flags_t *flags, char *buffer, int *current_symbol) {
  char null_str[7] = "(null)";
  int length = (int)s21_strlen(null_str);
  char reserve_buf[10] = {'\0'};

  if ((flags->precision_num > 0) && (flags->precision_num < length)) {
    for (int i = 0; i < flags->precision_num; i++) {
      reserve_buf[i] = null_str[i];
    }
    flags->width -= flags->precision_num;
  } else if (flags->precision_num != 0) {
    for (int j = 0; j < length; j++) {
      reserve_buf[j] = null_str[j];
    }
    flags->width -= length;
  }

  if ((flags->width > 0) && (flags->align_minus == 0)) {
    for (int k = 0; k < flags->width; k++) {
      buffer[*current_symbol] = ' ';
      *current_symbol = (*current_symbol) + 1;
    }
  }

  for (int l = 0; l < (int)s21_strlen(reserve_buf); l++) {
    buffer[*current_symbol] = reserve_buf[l];
    *current_symbol = (*current_symbol) + 1;
  }

  if ((flags->width > 0) && (flags->align_minus == 1)) {
    for (int m = 0; m < flags->width; m++) {
      buffer[*current_symbol] = ' ';
      *current_symbol = (*current_symbol) + 1;
    }
  }
  buffer[*current_symbol] = '\0';
}

void append_hex(va_list ap, flags_t *flags, long unsigned int v_x, char *array,
                int *current_symbol, const char *format) {
  int zero_flag = 0;

  if (flags->star_width == 1) {
    flags->width = va_arg(ap, int);
  }
  if (flags->star_precision) {
    flags->precision_num = va_arg(ap, int);
  }
  if (flags->precision_num == -1) flags->precision_num = 0;

  if (flags->len_h == 1) {
    v_x = va_arg(ap, int);
  } else if (flags->len_l == 1) {
    v_x = va_arg(ap, long int);
  } else {
    v_x = va_arg(ap, int);
  }

  v_x = (long int)v_x;
  int flag = (*format == 'X') ? 1 : 0;
  if (flags->precision_num > 0) {
    char *temp_ar = hex_to_string(v_x, flag, flags->hash_zero, 0);
    int len_helper = s21_strlen(temp_ar);
    int size_helper = (flags->hash_zero > 0) ? len_helper - 2 : len_helper;
    flags->precision_zero = (flags->precision_num - size_helper > 0)
                                ? (flags->precision_num - size_helper)
                                : 0;
    free(temp_ar);
  }

  int temp_shift = 0;
  char *temp = hex_to_string(v_x, flag, flags->hash_zero, 0);

  if (flags->precision_zero) {
    if (flags->width > 0 && flags->align_minus == 0) {
      add_space_precision_zero(temp, array, current_symbol, flags);
    }
    zero_flag =
        add_precision_zero(&temp, array, current_symbol, flags, 1, &temp_shift);
  } else if (flags->direct_zero) {
    zero_flag = add_zero(&temp, array, current_symbol, (int)s21_strlen(temp),
                         flags, &temp_shift);
  } else if (flags->width > 0 && flags->align_minus == 0) {
    add_space(temp, array, current_symbol, (int)s21_strlen(temp), flags);
  }
  int x_flag = (flags->hash_zero > 0) ? 1 : 0;
  buffer_writing(temp, array, current_symbol, (int)s21_strlen(temp), zero_flag,
                 x_flag, 0);
  if (flags->align_minus) {
    align_left(array, current_symbol, (int)s21_strlen(temp), flags->width,
               flags, temp);
  }
  free(temp - temp_shift);
}

void append_g(va_list ap, flags_t *flags, long double v_e, char *array,
              int *current_symbol, const char *format_pointer) {
  char *temp = S21_NULL;
  if (flags->star_width) {
    flags->width = va_arg(ap, int);
  }
  if (flags->star_precision) {
    flags->precision_num = va_arg(ap, int);
  }
  if (flags->precision_num == -1) flags->precision_num = 0;
  if (flags->length_double) {
    v_e = va_arg(ap, long double);
  } else {
    v_e = va_arg(ap, double);
  }
  int temp_shift = 0;
  if (v_e >= 0 && flags->show_sign == 1 && flags->width == 0) {
    array[*current_symbol] = '+';
    *current_symbol += 1;
  }
  if (v_e > 0 && flags->space_for_pos == 1 && flags->width == 0) {
    array[*current_symbol] = ' ';
    *current_symbol += 1;
  }
  if (v_e == 0) {
    array[*current_symbol] = '0';
    *current_symbol += 1;
  } else {
    long double pos_v_e = (v_e < 0) ? (v_e * -1) : (v_e);
    int check_exp = find_exp(pos_v_e);
    if (flags->dot_check == 1 && flags->precision_num == 0) {
      flags->precision_num = 1;
    }
    int control = check_precision(flags, check_exp);
    if (control == 1) {
      if (flags->precision_num == 0 && flags->dot_check == 0) {
        flags->precision_num = 5;
      } else if (flags->precision_num > 0) {
        flags->precision_num = flags->precision_num - 1;
      }
      temp = exp_to_string(v_e, format_pointer, 1, flags);
    } else {
      if (flags->precision_num == 0 && flags->dot_check == 0) {
        if (check_exp < 0) {
          check_exp *= -1;
          flags->precision_num = (6 - check_exp);
        } else {
          flags->precision_num = (6 - (check_exp + 1));
        }
      } else if (flags->precision_num == 0 && flags->dot_check == 1) {
        if (check_exp < 0) check_exp *= -1;
        flags->precision_num = check_exp;
      } else if (flags->precision_num > 0) {
        flags->precision_num = (flags->precision_num - (check_exp + 1));
      }
      temp = float_to_string(v_e, 1, flags);
    }
    int zero_flag = 0;
    if (flags->direct_zero) {
      zero_flag = add_zero(&temp, array, current_symbol, (int)s21_strlen(temp),
                           flags, &temp_shift);
    } else if (flags->width > 0 && flags->align_minus == 0) {
      add_space(temp, array, current_symbol, (int)s21_strlen(temp), flags);
    }
    buffer_writing(temp, array, current_symbol, (int)s21_strlen(temp),
                   zero_flag, 0, 0);
  }
  if (flags->align_minus) {
    align_left(array, current_symbol, (int)s21_strlen(temp), flags->width,
               flags, temp);
  }
  array[*current_symbol] = '\0';
  free(temp - temp_shift);
}

int find_exp(long double num) {
  int res = 0;
  if (num > 0 && num < 1) {
    while ((long int)num == 0) {
      num = num * 10;
      res++;
    }
    res *= -1;
  } else if (num > 1) {
    while (num != 0) {
      num = (long int)num / 10;
      res++;
    }
    res--;
  }
  return res;
}

void append_float(va_list ap, flags_t *flags, long double v_f, char *array,
                  int *current_symbol) {
  int zero_flag = 0;
  if (flags->star_width) {
    flags->width = va_arg(ap, int);
  }
  if (flags->star_precision) {
    flags->precision_num = va_arg(ap, int);
  }
  if (flags->precision_num == -1) flags->precision_num = 0;
  if (flags->length_double) {
    v_f = va_arg(ap, long double);
  } else {
    v_f = va_arg(ap, double);
  }
  if (flags->precision_num == 0 && flags->dot_check == 0) {
    flags->precision_num = 6;
  } else if (flags->precision_num == 0 && flags->dot_check == 1) {
    flags->precision_num = 0;
  }
  if (v_f > 0 && flags->show_sign == 1 && flags->width == 0) {
    array[*current_symbol] = '+';
    *current_symbol += 1;
  }
  if (v_f >= 0 && flags->space_for_pos == 1 && flags->width == 0) {
    array[*current_symbol] = ' ';
    *current_symbol += 1;
  }
  int temp_shift = 0;
  char *temp = float_to_string(v_f, 0, flags);
  if (flags->direct_zero == 1) {
    zero_flag = add_zero(&temp, array, current_symbol, (int)s21_strlen(temp),
                         flags, &temp_shift);
  } else if (flags->width > 0) {
    add_space(temp, array, current_symbol, (int)s21_strlen(temp), flags);
  }
  buffer_writing(temp, array, current_symbol, (int)s21_strlen(temp), zero_flag,
                 0, 0);
  if (flags->align_minus) {
    align_left(array, current_symbol, (int)s21_strlen(temp), flags->width,
               flags, temp);
  }
  free(temp - temp_shift);
}

void append_exp(va_list ap, flags_t *flags, long double v_e, char *array,
                int *current_symbol, const char *format_pointer) {
  int zero_flag = 0;
  if (flags->star_width) {
    flags->width = va_arg(ap, int);
  }
  if (flags->star_precision) {
    flags->precision_num = va_arg(ap, int);
  }
  if (flags->precision_num == -1) flags->precision_num = 0;
  if (flags->length_double) {
    v_e = va_arg(ap, long double);
  } else {
    v_e = va_arg(ap, double);
  }
  if (flags->dot_check == 1 && flags->precision_num == 0) {
    flags->precision_num = 0;
  } else if (flags->precision_num == 0 && flags->dot_check == 0) {
    flags->precision_num = 6;
  }
  if (v_e > 0 && flags->show_sign == 1 && flags->width == 0) {
    array[*current_symbol] = '+';
    *current_symbol += 1;
  }
  if (v_e >= 0 && flags->space_for_pos == 1 && flags->width == 0) {
    array[*current_symbol] = ' ';
    *current_symbol += 1;
  }
  int temp_shift = 0;
  char *temp = exp_to_string(v_e, format_pointer, 0, flags);
  if (flags->direct_zero) {
    zero_flag = add_zero(&temp, array, current_symbol, (int)s21_strlen(temp),
                         flags, &temp_shift);
  } else if (flags->width > 0 && flags->align_minus == 0) {
    add_space(temp, array, current_symbol, (int)s21_strlen(temp), flags);
  }
  buffer_writing(temp, array, current_symbol, (int)s21_strlen(temp), zero_flag,
                 0, 0);
  if (flags->align_minus) {
    align_left(array, current_symbol, (int)s21_strlen(temp), flags->width,
               flags, temp);
  }
  free(temp - temp_shift);
}

void append_char(va_list ap, flags_t *flags, char *array, int *current_symbol) {
  unsigned char v_char;
  wchar_t long_char;
  if (flags->star_width == 1) {
    flags->width = va_arg(ap, int);
  }
  if (flags->star_precision == 1) {
    flags->precision_num = va_arg(ap, int);
  }
  if (flags->len_l == 1) {
    long_char = va_arg(ap, wchar_t);
  } else {
    v_char = (unsigned char)va_arg(ap, int);
  }
  if (flags->len_l == 1) {
    wchar_t temp[2];
    temp[0] = long_char;
    temp[1] = '\0';
    int size_helper = sizeof(temp[0]);
    int add_size = wcstombs(array + (*current_symbol), temp, size_helper);
    if (flags->align_minus == 0 && flags->width > add_size) {
      for (int i = 0; i < (flags->width - add_size); i++) {
        array[*current_symbol] = ' ';
        *current_symbol += 1;
      }
    }
    add_size = wcstombs(array + (*current_symbol), temp, size_helper);
    *current_symbol += add_size;
    if (flags->align_minus == 1 && flags->width > add_size) {
      for (int i = 0; i < (flags->width - add_size); i++) {
        array[*current_symbol] = ' ';
        *current_symbol += 1;
      }
    }
  } else {
    if (flags->align_minus == 0 && flags->width > 1) {
      for (int i = 0; i < (flags->width - 1); i++) {
        array[*current_symbol] = ' ';
        *current_symbol += 1;
      }
    }
    array[*current_symbol] = v_char;
    *current_symbol += 1;
    if (flags->align_minus == 1 && flags->width > 1) {
      for (int i = 0; i < (flags->width - 1); i++) {
        array[*current_symbol] = ' ';
        *current_symbol += 1;
      }
    }
    array[*current_symbol] = '\0';
  }
}

void append_int(va_list ap, flags_t *flags, char *array, int *current_symbol,
                const char *gen_pointer) {
  long int v_int = 0;
  if (flags->star_width == 1) {
    flags->width = va_arg(ap, int);
  }
  if (flags->star_precision) {
    flags->precision_num = va_arg(ap, int);
  }
  if (flags->len_h == 1) {
    v_int = ((*gen_pointer) == 'u')
                ? (unsigned short int)va_arg(ap, unsigned int)
                : (short int)va_arg(ap, int);
  } else if (flags->len_l == 1) {
    v_int = ((*gen_pointer) == 'u') ? va_arg(ap, unsigned long int)
                                    : va_arg(ap, long int);
  } else if ((*gen_pointer) == 'u') {
    v_int = va_arg(ap, unsigned int);
  } else {
    v_int = va_arg(ap, int);
  }
  if ((*gen_pointer) == 'u') {
    flags->space_for_pos = 0;
    flags->show_sign = 0;
  }
  char *temp = s21_itoa(v_int);
  int temp_shift = 0;
  int sign = (v_int < 0) ? 1 : 0;
  if (v_int == 0 && flags->precision_num == 0) {
    array[*current_symbol] = '\0';
  } else {
    if (flags->precision_num == -1) flags->precision_num = 0;
    if (flags->precision_num > 0) {
      char *temp_ar = s21_itoa(v_int);
      int size_helper =
          (temp_ar[0] == '-') ? s21_strlen(temp_ar + 1) : s21_strlen(temp_ar);
      flags->precision_zero = (flags->precision_num - size_helper > 0)
                                  ? (flags->precision_num - size_helper)
                                  : 0;
      free(temp_ar);
    }
    if (v_int > 0 && flags->show_sign == 1 && flags->width == 0 &&
        flags->precision_zero == 0) {
      array[*current_symbol] = '+';
      *current_symbol += 1;
    }
    if (v_int >= 0 && flags->space_for_pos == 1 && flags->width == 0) {
      array[*current_symbol] = ' ';
      *current_symbol += 1;
    }
    int zero_flag = 0;
    if (flags->precision_zero) {
      if (flags->width > 0 && flags->align_minus == 0) {
        add_space_precision_zero(temp, array, current_symbol, flags);
      }
      zero_flag = add_precision_zero(&temp, array, current_symbol, flags, 0,
                                     &temp_shift);
      if (temp_shift) sign = 0;
    } else if (flags->direct_zero) {
      zero_flag = add_zero(&temp, array, current_symbol, (int)s21_strlen(temp),
                           flags, &temp_shift);
      if (temp_shift) sign = 0;
    } else if (flags->width > 0) {
      add_space(temp, array, current_symbol, (int)s21_strlen(temp), flags);
    }
    buffer_writing(temp, array, current_symbol, (int)s21_strlen(temp),
                   zero_flag, 0, sign);
  }
  if (flags->align_minus) {
    align_left(array, current_symbol,
               (int)s21_strlen(temp) + flags->precision_zero, flags->width,
               flags, temp);
  }
  array[*current_symbol] = '\0';
  free(temp - temp_shift);
}

int add_precision_zero(char **temp, char *main_buf, int *current_symbol,
                       flags_t *flags, int flag_x, int *shift) {
  int count = 0;
  if ((flags->width == 0) || (flags->align_minus == 1)) {
    if (((*temp)[0] != '-') && (flags->space_for_pos == 1)) {
      main_buf[*current_symbol] = ' ';
      *current_symbol += 1;
    } else if (((*temp)[0] != '-') && (flags->show_sign == 1)) {
      main_buf[*current_symbol] = '+';
      *current_symbol += 1;
    }
  }
  if (flags->precision_zero > 0 && flag_x == 0) {
    if ((*temp)[0] == '-') {
      main_buf[*current_symbol] = '-';
      *current_symbol += 1;
      for (int i = 0; i < flags->precision_zero; i++) {
        main_buf[*current_symbol] = '0';
        *current_symbol += 1;
        count++;
      }
      (*temp) += 1;
      (*shift) += 1;
    } else {
      for (int i = 0; i < flags->precision_zero; i++) {
        main_buf[*current_symbol] = '0';
        *current_symbol += 1;
      }
    }
  }
  if (flags->precision_zero > 0 && flag_x == 1) {
    if (flags->hash_zero) {
      main_buf[*current_symbol] = '0';
      *current_symbol += 1;
      main_buf[*current_symbol] = 'x';
      *current_symbol += 1;
    }
    for (int i = 0; i < flags->precision_zero; i++) {
      main_buf[*current_symbol] = '0';
      *current_symbol += 1;
      count++;
    }
  }
  return count;
}

void buffer_writing(const char *temp, char *main_buf, int *current_symbol,
                    int count, int flag, int hex_flag, int sign) {
  int i = 0;
  if (flag) {
    if (hex_flag) {
      i = 2;
    } else if (sign) {
      i = 1;
    }
  }
  for (; i < count; i++) {
    main_buf[*current_symbol] = temp[i];
    *current_symbol += 1;
  }
  main_buf[*current_symbol] = '\0';
}

void align_left(char *main_buf, int *current_symbol, int length, int fill,
                flags_t *flags, const char *temp) {
  if (flags->show_sign == 1 && temp[0] != '-') {
    length++;
  }
  if ((flags->space_for_pos == 1) && (temp[0] != '-')) {
    length++;
  }
  if (fill > length) {
    for (int i = 0; i < (fill - length); i++) {
      main_buf[*current_symbol] = ' ';
      *current_symbol += 1;
    }
    main_buf[*current_symbol] = '\0';
  }
}

int add_zero(char **temp, char *main_buf, int *current_symbol, int length,
             flags_t *flags, int *shift) {
  int count = 0;
  if (((*temp)[0] != '-') && (flags->show_sign == 1)) {
    length++;
    if (flags->width > 0) {
      main_buf[*current_symbol] = '+';
      *current_symbol += 1;
    }
  }
  if (flags->width > length) {
    if ((*temp)[0] == '-') {
      main_buf[*current_symbol] = '-';
      *current_symbol += 1;
      for (int i = 0; i < (flags->width - length); i++) {
        main_buf[*current_symbol] = '0';
        *current_symbol += 1;
        count++;
      }
      (*temp) += 1;
      (*shift) += 1;
    } else {
      for (int i = 0; i < (flags->width - length); i++) {
        main_buf[*current_symbol] = '0';
        *current_symbol += 1;
      }
    }
  }
  return count;
}

void add_space_precision_zero(char *temp, char *main_buf, int *current_symbol,
                              flags_t *flags) {
  int length = 0;
  if (flags->show_sign == 1 && temp[0] != '-') {
    length++;
  }
  length = flags->width - flags->precision_zero - s21_strlen(temp) - length;
  if (length > 0) {
    for (int i = 0; i < length; i++) {
      main_buf[*current_symbol] = ' ';
      *current_symbol += 1;
    }
    if ((temp[0] != '-') && (flags->show_sign == 1)) {
      main_buf[*current_symbol] = '+';
      *current_symbol += 1;
    }
  } else if ((temp[0] != '-') && (flags->space_for_pos == 1)) {
    main_buf[*current_symbol] = ' ';
    *current_symbol += 1;
  } else if ((temp[0] != '-') && (flags->show_sign == 1)) {
    main_buf[*current_symbol] = '+';
    *current_symbol += 1;
  }
}

void add_space(const char *temp, char *main_buf, int *current_symbol,
               int length, flags_t *flags) {
  if (flags->align_minus == 0) {
    if ((temp[0] != '-') && (flags->show_sign == 1)) {
      length++;
      if (flags->width <= length) {
        main_buf[*current_symbol] = '+';
        *current_symbol += 1;
      }
    }
    if (flags->width > length) {
      if (temp[0] == '-') {
        for (int i = 0; i < (flags->width - length); i++) {
          main_buf[*current_symbol] = ' ';
          *current_symbol += 1;
        }
      } else {
        for (int i = 0; i < (flags->width - length); i++) {
          main_buf[*current_symbol] = ' ';
          *current_symbol += 1;
        }
        if (flags->show_sign == 1) {
          main_buf[*current_symbol] = '+';
          *current_symbol += 1;
        }
      }
    } else if ((temp[0] != '-') && (flags->space_for_pos == 1)) {
      main_buf[*current_symbol] = ' ';
      *current_symbol += 1;
    }
  } else {
    if ((temp[0] != '-') && (flags->space_for_pos == 1)) {
      main_buf[*current_symbol] = ' ';
      *current_symbol += 1;
    } else if ((temp[0] != '-') && (flags->show_sign == 1)) {
      main_buf[*current_symbol] = '+';
      *current_symbol += 1;
    }
  }
}

int parse_params(const char *str_pointer, flags_t *flags, int *shift) {
  *shift += 1;
  int local_stop_flag = 0;
  while (!local_stop_flag) {
    str_pointer++;
    if (*str_pointer == '-') {
      flags->align_minus = 1;
    } else if (*str_pointer == '+') {
      flags->show_sign = 1;
    } else if (*str_pointer == ' ') {
      flags->space_for_pos = 1;
    } else if (*str_pointer == '#') {
      flags->hash_zero = 1;
    } else if (*str_pointer >= 48 && *str_pointer <= 57) {
      if (*str_pointer == '0') {
        if (flags->dot_check == 0) {
          if (flags->direct_zero == 0 && flags->width == 0) {
            flags->direct_zero = 1;
          } else {
            flags->width = flags->width * 10;
          }
        } else {
          flags->precision_num =
              (flags->precision_num > 0)
                  ? (flags->precision_num * 10 + (*str_pointer - '0'))
                  : ((*str_pointer) - '0');
        }
      } else {
        if (flags->dot_check == 1) {
          flags->precision_num =
              (flags->precision_num > 0)
                  ? (flags->precision_num * 10 + (*str_pointer - '0'))
                  : ((*str_pointer) - '0');
        } else {
          flags->width = (flags->width > 0)
                             ? ((flags->width * 10) + (*str_pointer - '0'))
                             : (*str_pointer) - '0';
        }
      }
    } else if ((*str_pointer) == '.' || (*str_pointer) == ',') {
      flags->dot_check = 1;
    } else if (*str_pointer == '*') {
      if (flags->dot_check == 1) {
        flags->star_precision = 1;
      } else {
        flags->star_width = 1;
      }
    } else if (*str_pointer == 'h') {
      flags->len_h = 1;
    } else if (*str_pointer == 'l') {
      flags->len_l = 1;
    } else if (*str_pointer == 'L') {
      flags->length_double = 1;
    } else if (s21_strchr(TOKENS, (int)(*str_pointer)) ||
               (*str_pointer == '%')) {
      break;
    } else {
      local_stop_flag = 1;
    }
    *shift += 1;
  }
  return local_stop_flag;
}

void init_struct(flags_t *res) {
  res->align_minus = 0;
  res->show_sign = 0;
  res->space_for_pos = 0;
  res->hash_zero = 0;
  res->direct_zero = 0;
  res->width = 0;
  res->star_width = 0;
  res->star_precision = 0;
  res->dot_check = 0;
  res->precision_num = -1;
  res->precision_zero = 0;
  res->len_h = 0;
  res->len_l = 0;
  res->length_double = 0;
}

char *s21_itoa(long int num) {
  char *res = malloc(sizeof(char) * 40);
  if (!res) exit(0);
  char *reverse = malloc(sizeof(char) * 40);
  if (!reverse) {
    free(res);
    exit(0);
  }
  int count = 0;
  int sign = 0;
  if (num < 0) {
    sign = 1;
    num *= -1;
  }
  if (num == 0) {
    res[count++] = '0';
    res[count++] = '\0';
  } else {
    int i;
    while (num > 0) {
      long int next = num % 10;
      reverse[count++] = '0' + next;
      num /= 10;
    }
    if (sign) {
      reverse[count++] = '-';
    }
    for (i = 0; i < count; i++) {
      res[i] = reverse[count - i - 1];
    }
    res[i] = '\0';
  }
  free(reverse);
  return res;
}

void delete_zero(char *g_string, int *current_symbol) {
  while (g_string[(*current_symbol) - 1] == '0') {
    g_string[(*current_symbol) - 1] = '\0';
    (*current_symbol) -= 1;
  }
  if (g_string[(*current_symbol) - 1] == conflicting_char) {
    g_string[(*current_symbol) - 1] = '\0';
    (*current_symbol) -= 1;
  }
}

char *hex_to_string(unsigned long int num, int upper_flag, int pointer_flag,
                    int nil_flag) {
  char *res = malloc(sizeof(char) * 50);
  if (!res) exit(0);
  char *reverse = malloc(sizeof(char) * 50);
  if (!reverse) {
    free(res);
    exit(0);
  }
  if (num == 0) {
    if (nil_flag) {
      for (int i = 0; i < 3; i++) {
        res[i] = "0x0"[i];
      }
      res[3] = '\0';
    } else {
      res[0] = '0';
      res[1] = '\0';
    }
  } else {
    int count = 0;
    int i = 0;
    while (num > 0) {
      long int next = num % 16;
      if (next > 9) {
        if (upper_flag) {
          reverse[count++] = 'A' + next - 10;
        } else {
          reverse[count++] = 'a' + next - 10;
        }
      } else {
        reverse[count++] = next + '0';
      }
      num /= 16;
    }
    reverse[count] = '\0';
    char *first = s21_strstr(reverse, "ffffffff");
    char *second = s21_strstr(reverse, "FFFFFFFF");
    if (first != S21_NULL || second != S21_NULL) {
      count = count - 8;
    }
    if (pointer_flag) {
      if (upper_flag) {
        reverse[count++] = 'X';
        reverse[count++] = '0';
      } else {
        reverse[count++] = 'x';
        reverse[count++] = '0';
      }
    }
    for (; i < count; i++) {
      res[i] = reverse[count - i - 1];
    }
    res[i] = '\0';
  }
  free(reverse);
  return res;
}

char *float_to_string(long double num, int g_flag, flags_t *flags) {
  char *res = malloc(sizeof(char) * 50);
  if (!res) exit(0);
  char *reverse = S21_NULL;
  int current_symbol = 0;
  int loc_precision = flags->precision_num;
  int sign_flag = 0;

  if (num < 0) {
    res[current_symbol] = '-';
    num *= -1;
    current_symbol++;
    sign_flag = 1;
  }
  if (num != 0 && log10(num) >= 0) {
    reverse = s21_itoa((long int)num);
    for (int j = 0; j < (int)s21_strlen(reverse); j++) {
      res[current_symbol] = reverse[j];
      current_symbol++;
    }
    free(reverse);
  } else {
    res[current_symbol] = '0';
    current_symbol++;
  }
  res[current_symbol] = conflicting_char;
  current_symbol++;
  int fraction = check_fraction(&current_symbol, flags, num);
  long double int_part;
  long double frac_part = modfl(num, &int_part);
  if (fraction == 1) {
    int indicator = 0;
    long double next;
    for (int k = 0; k < (loc_precision + 1); k++) {
      frac_part *= 10;
      frac_part = modfl(frac_part, &next);
      res[current_symbol] = (int)next + '0';
      current_symbol++;
    }
    if (res[current_symbol - 1] >= 53) {
      res[current_symbol - 2] =
          ((res[current_symbol - 2] - '0') + 1) % 10 + '0';
      indicator++;
    }
    res[current_symbol - 1] = '\0';
    current_symbol--;
    if (indicator && (res[current_symbol - 1] == '0')) {
      int helper;
      int reminder = 1;
      for (int i = current_symbol - 2; i >= sign_flag; i--) {
        if (res[i] == '.' || res[i] == ',') {
          continue;
        }
        helper = reminder + res[i] - '0';
        res[i] = (helper % 10) + '0';
        reminder = helper / 10;
      }
    }
  } else {
    if ((long long int)round(num) - (long int)int_part > 0) {
      int helper;
      int reminder = 0;
      helper = reminder + res[current_symbol - 1] - '0';
      res[current_symbol - 1] = ((helper + 1) % 10) + '0';
      reminder = (helper + 1) / 10;
      for (int i = current_symbol - 2; i >= sign_flag; i--) {
        helper = reminder + res[i] - '0';
        res[i] = (helper % 10) + '0';
        reminder = helper / 10;
      }
    }
  }
  if (g_flag == 1 && flags->hash_zero == 0) {
    delete_zero(res, &current_symbol);
  }

  res[current_symbol] = '\0';
  current_symbol++;
  return res;
}

char *exp_to_string(long double v_e, const char *exp_letter, int g_flag,
                    flags_t *flags) {
  char *res = malloc(sizeof(char) * 50);
  if (!res) exit(0);
  int current_symbol = 0;
  char exp_sign = '+';
  int loc_precision = flags->precision_num;
  char letter = (*exp_letter == 'E' || *exp_letter == 'G') ? 'E' : 'e';
  int main_sign = 0;

  if (v_e < 0) {
    res[current_symbol] = '-';
    v_e *= -1;
    current_symbol++;
    main_sign = 1;
  }
  int v_log = 0;
  if (v_e > 0) {
    v_log = find_exp(v_e);
  }
  if (v_e > 0 && v_e < 1) {
    exp_sign = '-';
  }
  long double num_to_save = (v_e / pow(10, v_log));
  res[current_symbol] = '0' + (int)num_to_save;
  current_symbol++;
  res[current_symbol] = conflicting_char;
  current_symbol++;
  long double int_part;
  long double frac_part = modfl(num_to_save, &int_part);
  int fraction = check_fraction(&current_symbol, flags, num_to_save);
  if (fraction == 1) {
    int indicator = 0;
    long double next;
    for (int i = 0; i < loc_precision + 1; i++) {
      frac_part *= 10;
      frac_part = modfl(frac_part, &next);
      res[current_symbol] = (int)next + '0';
      current_symbol++;
    }
    if (res[current_symbol - 1] >= 53) {
      res[current_symbol - 2] =
          ((res[current_symbol - 2] - '0') + 1) % 10 + '0';
      indicator++;
    }
    res[current_symbol - 1] = '\0';
    current_symbol--;
    if (indicator && (res[current_symbol - 1] == '0')) {
      int helper;
      int reminder = 1;
      for (int i = current_symbol - 2; i >= main_sign; i--) {
        if (res[i] == ',' || res[i] == '.') {
          continue;
        }
        helper = reminder + res[i] - '0';
        res[i] = (helper % 10) + '0';
        reminder = helper / 10;
      }
    }
    if (g_flag == 1 && flags->hash_zero == 0) {
      delete_zero(res, &current_symbol);
    }
  } else {
    if ((long long int)round(num_to_save) - (long int)int_part > 0) {
      int helper2 = res[0] - '0';
      res[0] = ((helper2 + 1) % 10) + '0';
    }
  }
  res[current_symbol] = letter;
  current_symbol++;
  res[current_symbol] = exp_sign;
  current_symbol++;
  if (v_log < 10 && v_log > -10) {
    res[current_symbol] = '0';
    current_symbol++;
  }
  char *exp_string = s21_itoa(v_log);
  int i = (v_log >= 0) ? 0 : 1;
  for (; i < (int)s21_strlen(exp_string); i++) {
    res[current_symbol] = exp_string[i];
    current_symbol++;
  }
  free(exp_string);
  res[current_symbol] = '\0';
  return res;
}

int check_fraction(int *current_symbol, flags_t *flags, long double num) {
  int total;
  long int check = (long int)num;
  if ((num - check) == 0) {
    if (flags->precision_num == 0 && flags->dot_check == 1 &&
        flags->hash_zero == 1) {
      total = 0;
    } else if (flags->precision_num == 0 && flags->dot_check == 1 &&
               flags->hash_zero == 0) {
      total = 0;
      *current_symbol -= 1;
    } else {
      total = 1;
    }
  } else if (flags->dot_check == 1 && flags->precision_num == 0) {
    if (flags->hash_zero == 0) {
      total = 0;
      *current_symbol -= 1;
    } else {
      total = 0;
    }
  } else {
    total = 1;
  }
  return total;
}

int check_precision(flags_t *flags, int expo) {
  int res = 2;
  if (flags->precision_num == 0) {
    if (expo < -4 || expo >= 6) {
      res = 1;
    }
  } else if (flags->precision_num > 0) {
    if (expo < -4 || expo >= flags->precision_num) {
      res = 1;
    }
  }
  return res;
}
