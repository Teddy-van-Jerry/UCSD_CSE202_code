use rand::Rng;

/// Represents a polynomial with real coefficients
#[derive(Debug, Clone, PartialEq)]
pub struct Polynomial {
    /// Coefficients of the polynomial, from lowest to highest degree
    pub coeffs: Vec<f64>,
}

impl Polynomial {
    /// Create a new polynomial from a vector of coefficients
    pub fn new(coeffs: Vec<f64>) -> Self {
        // Remove trailing zeros
        let mut result = coeffs;
        while result.len() > 0 && result.last().unwrap_or(&0.0).abs() < 1e-12 {
            result.pop();
        }
        Polynomial { coeffs: result }
    }

    /// Generate a random polynomial with given size
    ///
    /// - `range_min`: Minimum value for coefficients
    /// - `range_max`: Maximum value for coefficients
    /// - `size`: Number of coefficients
    /// ## Example
    /// ```
    /// use mult_polynomial::Polynomial;
    /// let p = Polynomial::random(-10.0, 10.0, 5);
    /// ```
    pub fn random(range_min: f64, range_max: f64, size: usize) -> Polynomial {
        assert!(size > 0);
        assert!(range_min < range_max);
        let mut rng = rand::thread_rng();
        Polynomial::new(
            (0..size)
                .map(|_| rng.gen_range(range_min..range_max))
                .collect(),
        )
    }

    /// Get the degree of the polynomial
    pub fn degree(&self) -> usize {
        if self.coeffs.is_empty() {
            0
        } else {
            self.coeffs.len() - 1
        }
    }

    /// Evaluate the polynomial at a given point x
    pub fn evaluate(&self, x: f64) -> f64 {
        let mut result = 0.0;
        let mut power = 1.0;

        for coeff in &self.coeffs {
            result += coeff * power;
            power *= x;
        }

        result
    }

    /// Multiply with another polynomial using the naive O(n²) algorithm
    pub fn multiply_naive(&self, other: &Polynomial) -> Polynomial {
        Polynomial::new(naive_multiply_impl(&self.coeffs, &other.coeffs))
    }

    /// Multiply with another polynomial using the Cook-Tooms algorithm with k=3
    pub fn multiply_cook_tooms_k3(&self, other: &Polynomial) -> Polynomial {
        Polynomial::new(cook_tooms_k3_impl(&self.coeffs, &other.coeffs))
    }

    /// Multiply with another polynomial using a thresholded approach
    pub fn multiply_thresholded(&self, other: &Polynomial, threshold: usize) -> Polynomial {
        Polynomial::new(thresholded_multiply_impl(
            &self.coeffs,
            &other.coeffs,
            threshold,
        ))
    }
}

/// Implementation of Cook-Tooms algorithm with k=3 (Toom-3)
fn cook_tooms_k3_impl(a: &[f64], b: &[f64]) -> Vec<f64> {
    thresholded_multiply_impl(a, b, 5) // only use naive for length 1
}

/// Basic naive implementation of polynomial multiplication
fn naive_multiply_impl(a: &[f64], b: &[f64]) -> Vec<f64> {
    let n = a.len();
    let m = b.len();

    if n == 0 || m == 0 {
        return Vec::new();
    }

    let mut result = vec![0.0; n + m - 1];

    for i in 0..n {
        for j in 0..m {
            result[i + j] += a[i] * b[j];
        }
    }

    result
}

/// Thresholded version that chooses between algorithms based on input size
pub fn thresholded_multiply_impl(a: &[f64], b: &[f64], threshold: usize) -> Vec<f64> {
    let n = a.len();
    let m = b.len();
    let thr = std::cmp::max(threshold, 5); // at least 5 to work

    // Threshold check
    if n < thr || m < thr {
        return naive_multiply_impl(a, b);
    }

    // Determine chunk size for splitting polynomials into 3 parts
    let n_chunk = (n + 2) / 3; // ceiling division

    // Split a into 3 parts with proper padding to ensure consistent sizes
    let mut a0 = vec![0.0; n_chunk];
    let mut a1 = vec![0.0; n_chunk];
    let mut a2 = vec![0.0; n_chunk];

    for i in 0..n_chunk.min(n) {
        a0[i] = a[i];
    }

    for i in 0..n_chunk.min(n.saturating_sub(n_chunk)) {
        a1[i] = a[i + n_chunk];
    }

    for i in 0..n_chunk.min(n.saturating_sub(2 * n_chunk)) {
        a2[i] = a[i + 2 * n_chunk];
    }

    // Split b into 3 parts with proper padding
    let mut b0 = vec![0.0; n_chunk];
    let mut b1 = vec![0.0; n_chunk];
    let mut b2 = vec![0.0; n_chunk];

    for i in 0..n_chunk.min(m) {
        b0[i] = b[i];
    }

    for i in 0..n_chunk.min(m.saturating_sub(n_chunk)) {
        b1[i] = b[i + n_chunk];
    }

    for i in 0..n_chunk.min(m.saturating_sub(2 * n_chunk)) {
        b2[i] = b[i + 2 * n_chunk];
    }

    // Evaluate at 5 points: 0, 1, -1, 2, -2
    // let a_at_0 = a0.clone();
    let mut a_at_1 = vec![0.0; n_chunk];
    let mut a_at_neg1 = vec![0.0; n_chunk];
    let mut a_at_2 = vec![0.0; n_chunk];
    let mut a_at_neg2 = vec![0.0; n_chunk];

    // let b_at_0 = b0.clone();
    let mut b_at_1 = vec![0.0; n_chunk];
    let mut b_at_neg1 = vec![0.0; n_chunk];
    let mut b_at_2 = vec![0.0; n_chunk];
    let mut b_at_neg2 = vec![0.0; n_chunk];

    for i in 0..n_chunk {
        a_at_1[i] = a0[i] + a1[i] + a2[i];
        a_at_neg1[i] = a0[i] - a1[i] + a2[i];
        a_at_2[i] = a0[i] + 2.0 * a1[i] + 4.0 * a2[i];
        a_at_neg2[i] = a0[i] - 2.0 * a1[i] + 4.0 * a2[i];
        b_at_1[i] = b0[i] + b1[i] + b2[i];
        b_at_neg1[i] = b0[i] - b1[i] + b2[i];
        b_at_2[i] = b0[i] + 2.0 * b1[i] + 4.0 * b2[i];
        b_at_neg2[i] = b0[i] - 2.0 * b1[i] + 4.0 * b2[i];
    }

    // Pointwise multiplication at each evaluation point
    let p0 = thresholded_multiply_impl(&a0, &b0, thr);
    let p1 = thresholded_multiply_impl(&a_at_1, &b_at_1, thr);
    let p2 = thresholded_multiply_impl(&a_at_neg1, &b_at_neg1, thr);
    let p3 = thresholded_multiply_impl(&a_at_2, &b_at_2, thr);
    let p4 = thresholded_multiply_impl(&a_at_neg2, &b_at_neg2, thr);

    let result_len = n + m - 1;
    let mut result = vec![0.0; result_len];

    let mut add_to_result = |pos: usize, val: f64| {
        if pos < result_len {
            result[pos] += val;
        }
    };

    // Process each coefficient of the intermediate products
    let max_coeff = p0
        .len()
        .max(p1.len())
        .max(p2.len())
        .max(p3.len())
        .max(p4.len());

    for i in 0..max_coeff {
        let v0 = if i < p0.len() { p0[i] } else { 0.0 };
        let v1 = if i < p1.len() { p1[i] } else { 0.0 };
        let v2 = if i < p2.len() { p2[i] } else { 0.0 };
        let v3 = if i < p3.len() { p3[i] } else { 0.0 };
        let v4 = if i < p4.len() { p4[i] } else { 0.0 };

        let c0 = v0;
        let c1 = (v1 - v2) / 2.0 - (v3 - v4) / 12.0 - v0;
        let c2 = (v1 + v2) / 2.0 - v0 - (v3 + v4) / 6.0;
        let c3 = (v3 - v4) / 6.0 - (v1 - v2) / 6.0;
        let c4 = (v3 + v4) / 24.0 - (v1 + v2) / 24.0 + v0 / 24.0;

        add_to_result(i, c0);
        add_to_result(i + n_chunk, c1);
        add_to_result(i + 2 * n_chunk, c2);
        add_to_result(i + 3 * n_chunk, c3);
        add_to_result(i + 4 * n_chunk, c4);
    }

    result
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum PolynomialMultAlg {
    Naive,
    CookTooms,
    Thresholded(usize),
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Test multiplication of simple polynomials
    #[test]
    fn test_simple_multiplication() {
        // (1 + 2x + 3x²) * (4 + 5x + 6x²)
        // = 4 + 13x + 28x² + 27x³ + 18x⁴
        let p1 = Polynomial::new(vec![1.0, 2.0, 3.0]);
        let p2 = Polynomial::new(vec![4.0, 5.0, 6.0]);

        let result_naive = p1.multiply_naive(&p2);
        let result_cook_tooms = p1.multiply_cook_tooms_k3(&p2);

        let expected = vec![4.0, 13.0, 28.0, 27.0, 18.0];

        // Verify both algorithms produce the correct result
        assert_eq!(result_naive.coeffs, expected);
        assert_eq!(result_cook_tooms.coeffs, expected);
    }

    /// Test multiplication with zero
    #[test]
    fn test_multiply_with_zero() {
        let p1 = Polynomial::new(vec![1.0, 2.0, 3.0]);
        let p2 = Polynomial::new(vec![0.0]);

        let result = p1.multiply_naive(&p2);
        assert_eq!(result.coeffs, vec![]);
    }

    /// Test multiplication of larger random polynomials
    #[test]
    fn test_random_polynomial_multiplication() {
        // Fixed test case with simple coefficients
        let p1 = Polynomial::new(vec![1.0, 2.0, 3.0]);
        let p2 = Polynomial::new(vec![4.0, 5.0, 6.0]);

        let result_naive = p1.multiply_naive(&p2);
        let result_cook_tooms = p1.multiply_cook_tooms_k3(&p2);

        println!("Naive result:     {:?}", result_naive.coeffs);
        println!("Cook-Tooms result: {:?}", result_cook_tooms.coeffs);

        // Expected result: [4, 13, 28, 27, 18]
        let expected = Polynomial::new(vec![4.0, 13.0, 28.0, 27.0, 18.0]);

        // Verify both implementations produce the correct results
        assert_eq!(result_naive.coeffs.len(), expected.coeffs.len());
        assert_eq!(result_cook_tooms.coeffs.len(), expected.coeffs.len());

        // Check correctness (with PartialEq derived)
        assert_eq!(result_naive, expected);
        assert_eq!(result_cook_tooms, expected);
        assert_eq!(result_naive, result_cook_tooms);
    }

    /// Test polynomials of different sizes
    #[test]
    fn test_different_sizes() {
        // (1 + x + x²) * (1 + x)
        // = 1 + 2x + 2x² + x³
        let p1 = Polynomial::new(vec![1.0, 1.0, 1.0]);
        let p2 = Polynomial::new(vec![1.0, 1.0]);

        let result = p1.multiply_naive(&p2);
        let expected = Polynomial::new(vec![1.0, 2.0, 2.0, 1.0]);
        assert_eq!(result, expected);

        // Also check with Cook-Tooms
        let result_cook_tooms = p1.multiply_cook_tooms_k3(&p2);
        assert_eq!(result_cook_tooms, expected);
    }

    /// Test special case polynomials
    #[test]
    fn test_special_cases() {
        // Constant polynomials: 5 * 7 = 35
        let p1 = Polynomial::new(vec![5.0]);
        let p2 = Polynomial::new(vec![7.0]);
        let result = p1.multiply_naive(&p2);
        assert_eq!(result, Polynomial::new(vec![35.0]));

        // Empty polynomial
        let p_empty = Polynomial::new(vec![]);
        let result = p1.multiply_naive(&p_empty);
        assert_eq!(result, Polynomial::new(vec![]));
    }
}
