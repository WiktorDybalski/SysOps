int collatz_conjecture(int input) {
    if (input % 2 == 0) {
        return input / 2;
    } else {
        return input * 3 + 1;
    }
}

int test_collatz_convergence(int input, int max_iter) {
    int i;
    for (i = 0; i < max_iter; i++) {
        if (input == 1) {
            return i + 1;
        }
        input = collatz_conjecture(input);
    }
    if (input != 1) {
        return -1;
    }
    return i;
}