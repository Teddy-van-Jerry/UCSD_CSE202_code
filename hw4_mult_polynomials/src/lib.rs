//! # Polynomial Multiplication Algorithms
//!
//! ## Implementation
//! Three polynomial multiplication algorithms are implemented:
//! 1. Naive algorithm
//! 2. Cook-Tooms algorithm
//! 3. Thresholded Cook-Tooms algorithm
//!
//! Internally, the Cook-Tooms algorithm is implemented as a special case of the thresholded version.
//! The threshold is set as 5 to keep the algorithm working in a simple way.
//! As such, any threshold equal or less than 5 will be the same as the Cook-Tooms algorithm.
//!
//! ## Author
//! Written by [Wuqiong Zhao](https://wqzhao.org).

mod polynomial;
pub use polynomial::thresholded_multiply_impl;
pub use polynomial::{Polynomial, PolynomialMultAlg};
