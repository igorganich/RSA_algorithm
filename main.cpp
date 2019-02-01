#include <iostream>
#include "biginteger.h"
#include <ctime>
#include <cstdlib>

using cus_int = wide_integer::generic_template::uint8192_t;
using namespace std;

cus_int pow(cus_int f, cus_int s)
{
	cus_int smth("1");
	int i = -1;
	while (++i < s)
		smth = smth * f;
	return (smth);
}

cus_int pow_mod(cus_int in_base, cus_int power, cus_int modulus){
    if (power == 0){
        return 1;
    }
    cus_int y = 1;
    cus_int base = in_base;
    while (power > 1){
        if (power % 2 == 0){
            base = ((base % modulus) * (base % modulus)) % modulus;
            power = power / 2;
        } else {
            y = (y * base) % modulus;
            base = ((base % modulus) * (base % modulus)) % modulus;
            power = (power - 1)/2;
        }
    }
    return ((base % modulus) * (y % modulus)) % modulus;;
}

int miller(cus_int n, int tests, int len)
{
	cus_int len2 = len;
	if ((n % 3) == 0 || (n % 5) == 0 || (n % 7) == 0 || (n % 11) == 0 || (n % 13) == 0)
		return (0);
	cus_int s;
	cus_int d;
	for (s = 0, d = n - 1; (d & 1) == 0;)
	{
		s += 1;
		d /= 2;
	}
	char *bits = new char[len + 1];
	for (int l = 0; l < tests; l++)
	{
		bits[len] = 0;
		for (int i = 0; i < len; i++)
			bits[i] = rand() % 2;
		for (int i = 0; i < len; i++)
			bits[i] += '0';
		cus_int temp("1");
		for (int i = 0; i < len; i++)
			if (bits[i] == '1')
				temp = temp + pow(2, len2 - i);
		cus_int test_result = pow_mod(temp, d, n);
		if (test_result == 1 || test_result == n - 1)
			continue;
		cus_int j("1");
		for (; j < s; j++)
		{
			test_result = pow_mod(test_result, 2, n);
			if (test_result == 1)
				return (0);
			if (test_result == n - 1)
			{
				break;
			}
		}
		if (j == s)
		{
			delete bits;
			return 0;
		}
	}
	delete bits;
	return 1;
}

cus_int create_prime_for_abon(int len, int tests)
{
	cus_int max;
	cus_int len2 = len + 1;
	max = pow(2, len2);
	char *bits = new char[len + 1];
	bits[0] = 1;
	bits[len] = '\0';
	for (int i = 1; i < len; i++)
		bits[i] = rand() % 2;
	bits[len - 1] = 1;
	for (int i = 0; i < len; i++)
		bits[i] += '0';
	cus_int temp("1");
	for (int i = 0; i < len; i++)
		if (bits[i] == '1')
			temp = temp + pow(2, len2 - 1 - i);
	for (; temp < max; temp += 2)
	{
		if (miller(temp, tests, len - 1) == 1)
			break;
	}
	if (temp > max)
		temp = create_prime_for_abon(len, tests);
	delete bits;
	return temp;
}

void swap_abon(cus_int *f, cus_int *s)
{
	cus_int t1 = f[0];
	cus_int t2 = f[1];
	f[0] = s[0];
	f[1] = s[1];
	s[0] = t1;
	s[1] = t2;
}

void get_primes(cus_int *abonA, cus_int *abonB, int len, int tests)
{
	abonA[0] = create_prime_for_abon(len, tests);
	abonA[1] = create_prime_for_abon(len, tests);
	abonB[0] = create_prime_for_abon(len, tests);
	abonB[1] = create_prime_for_abon(len, tests);
	if ((abonA[0] * abonA[1]) > (abonB[0] * abonB[1]))
		swap_abon(abonA, abonB);
	cout << abonA[0] << endl<< abonA[1] << endl<< abonB[0] << endl<< abonB[1] << endl;
}

int extended_euclid(cus_int a, cus_int b, cus_int * x, cus_int * y, cus_int * d)
{
	int flag = 0;
	cus_int q; cus_int r; cus_int x1;cus_int x2;cus_int y1; cus_int y2;
	if (b == 0) 
	{
		*d = a, *x = 1, *y = 0;
		return 2;
	}
	x2 = 1, x1 = 0, y2 = 0, y1 = 1;
	while (b > 0)
	{
		q = a / b, r = a - q * b;
		if (flag == 0)
		{
			*x = x2 - q * x1;
			flag++;
		}
		else if (flag == 1)
		{
			*x = q * x1 + x2;
			flag++;
		}
		else
		{
			*x = q * x1 + x2;
			flag--;
		}
		*y = y2 - q * y1;
		a = b, b = r;
		x2 = x1, x1 = *x, y2 = y1, y1 = *y;
	}
	*d = a, *x = x2, *y = y2;
	return flag;
}

cus_int inverse_unsigned(cus_int a, cus_int n)
{
  cus_int d, x, y;
  int flag = extended_euclid(a, n, & x, & y, & d);
  if (flag == 1)
  	x = n - x;
  if (d == 1) return x;
  return 0;
}


void generate_keys(cus_int *abon, cus_int *keys)
{
	cus_int e("65537");
	cus_int phi = (abon[0] - 1) * (abon[1] - 1);
	cus_int n = abon[0] * abon[1];
	cus_int d = inverse_unsigned(e, phi);
	d = d % phi;
	keys[0] = e;
	keys[1] = n;
	keys[2] = d;
}

cus_int create_message(int len)
{
	char *str = new char[len + 1];
	str[0] = rand() % 9 + '1';
	str[len] = 0;
	for (int i = 1; i < len; i++)
		str[i] = rand() % 10 + '0';
	cus_int ret(str);
	return ret;
}

cus_int encrypt(cus_int message, cus_int e, cus_int n)
{
	return pow_mod(message, e, n);
}

cus_int decrypt(cus_int encrypted, cus_int d, cus_int n)
{
	return pow_mod(encrypted, d, n);
}

cus_int sign(cus_int message, cus_int d, cus_int n)
{
	return pow_mod(message, d, n);
}

cus_int verify(cus_int sign, cus_int e, cus_int n)
{
	return pow_mod(sign, e, n);
}

int		nbr_inbase(char c, int base)
{
	if (base <= 10)
		return (c >= '0' && c <= '9');
	return ((c >= '0' && c <= '9') || (c >= 'A' && c <= ('A' + base - 10)) || \
	(c >= 'a' && c <= ('a' + base - 10)));
}

cus_int		atoi_base(const char *str, int base)
{
	int		i;
	cus_int	nbr;
	cus_int	sign;

	if (!str[0] || (base < 2 || base > 16))
		return (0);
	nbr = 0;
	sign = 1;
	while (str[i] == '\t' || str[i] == '\v' || str[i] == '\n' || \
		str[i] == ' ' || str[i] == '\r' || str[i] == '\f')
		i += 1;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			sign *= -1;
	while (str[i] && nbr_inbase(str[i], base))
	{
		if (str[i] >= 'A' && 'F' >= str[i])
			nbr = (nbr * base) + (str[i] - 'A' + 10);
		else if (str[i] >= 'a' && 'f' >= str[i])
			nbr = (nbr * base) + (str[i] - 'a' + 10);
		else
			nbr = (nbr * base) + (str[i] - '0');
		i += 1;
	}
	return (nbr * sign);
}

int		ft_abs(int nb)
{
	if (nb < 0)
		nb = -nb;
	return (nb);
}

char	*itoa_base(cus_int value, cus_int base)
{
	char	*str;
	int		size;
	string	tab;
	int		flag;
	cus_int		tmp;

	flag = 0;
	size = 0;
	tab = "0123456789ABCDEF";
	if (base < 2 || base > 16)
		return (0);
	if (value < 0 && base == 10)
		flag = 1;
	tmp = value;
	while ((tmp /= base) != 0)
		size++;
	size = size + flag + 1;
	str = (char*)malloc(sizeof(char) * size  + 1);
	str[size] = '\0';
	if (flag == 1)
		str[0] = '-';
	while (size > flag)
	{
		str[size - 1] = tab[ft_abs((int)(value % base))];
		size--;
		value /=base;
	}
	return (str);
}

void sendkey(cus_int n1, cus_int d1, cus_int e2, cus_int n2, cus_int *SK_b)
{
	cus_int key = create_message(20);
	cus_int Sign = sign(key, d1, n1);
	cus_int rets = encrypt(Sign, e2, n2);
	cus_int retk = encrypt(rets, e2, n2);
	SK_b[0] = rets;
	SK_b[1] = retk;
}

void receive_key(cus_int K1, cus_int sign, cus_int my_d, cus_int my_n, cus_int pub_site, cus_int site_n)
{
	cus_int K = decrypt(K1, my_d, my_n);
	cus_int S = decrypt(sign, my_d, my_n);
	cus_int s_verify = verify(S, pub_site, site_n);
	cout << "s_verify " << s_verify << endl << "K " << K <<endl;
}

int main()
{
	srand(time(NULL));
	int len;
	int tests;
	cus_int message;
	cus_int generatedA[3];
	cus_int generatedB[3];
	cus_int abonA[2];
	cus_int abonB[2];
	cout << "enter number of bits" << endl;
	cin >> len;
	cout << "enter num of test\n";
	cin >> tests;
	get_primes(abonA, abonB, len, tests);
	generate_keys(abonA, generatedA);
	generate_keys(abonB, generatedB);
	cout << "pub exponent - " << generatedA[0] << endl;
	cout << "n - " << generatedA[1] << endl;
	cout << "nhex - " << itoa_base(generatedA[1], 16) << endl;
	cout << "d - " << generatedA[2] << endl;
	cout << "pub exponent - " << generatedB[0] << endl;
	cout << "n - " << generatedB[1] << endl;
	cout << "d - " << generatedB[2] << endl;
	message = create_message(10);
	cout << "our message " << message << endl;
	cus_int encrypted = encrypt(message, generatedA[0], generatedA[1]);
	cout << "encrypted " << encrypted << endl;
	cus_int decrypted = decrypt(encrypted, generatedA[2], generatedA[1]);
	cout << "decrypted " << decrypted << endl;
	cus_int Sign = sign(message, generatedA[2], generatedA[1]);
	cout << "our sign " << Sign << endl;
	cus_int Verify = verify(Sign, generatedA[0], generatedA[1]);
	cout << "verify " << Verify << endl;
	return 0;
}
