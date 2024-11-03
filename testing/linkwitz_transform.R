# Calculate Linkwitz Transform Biquad Coefficients
#'
#' This function calculates the biquad filter coefficients for a Linkwitz transform.
#' It transforms the response of a sealed box speaker to match that of another
#' sealed box speaker with different parameters, typically with a lower F0.
#'
#' @param f0 Original resonance frequency of the sealed box speaker
#' @param q0 Original Q factor of the sealed box speaker
#' @param fp Desired (new) resonance frequency after transformation
#' @param qp Desired (new) Q factor after transformation
#' @param fs Sampling frequency
#'
#' @return A list containing the biquad filter coefficients (b0, b1, b2, a0, a1, a2)
#'
linkwitz_transform_biquad <- function(f0, q0, fp, qp, fs) {
  # Analog domain coefficients
  d0i = (2*pi*f0)^2
  d1i = (2*pi*f0)/q0 
  d2i = 1

  c0i = (2*pi*fp)^2
  c1i = (2*pi*fp)/qp
  c2i = 1

  fc = (f0 + fp) / 2
  gn = (2*pi*fc)/(tan(pi*fc/fs))
  cci = c0i+gn*c1i+gn*gn*c2i

  # Digital domain coefficients
  a0 = 1
  a1 = (2*(c0i-(gn^2)*c2i)/cci)
  a2 = ((c0i-gn*c1i+(gn^2)*c2i)/cci)

  b0 = (d0i+gn*d1i+(gn^2)*d2i)/cci
  b1 = 2*(d0i-(gn^2)*d2i)/cci
  b2 = (d0i-gn*d1i+(gn^2)*d2i)/cci
  
  # Return coefficients as a list
  return(list(b0 = b0, b1 = b1, b2 = b2, a0 = 1, a1 = a1, a2 = a2))
}

# Calculate frequency response of biquad filter
calculate_frequency_response <- function(b0, b1, b2, a0, a1, a2, fs, num_points = 1000) {
  f <- seq(10, 1000, length.out = num_points)
  w <- 2 * pi * f / fs
  z <- exp(1i * w)
  z_inv <- 1/z

  H <- (b0 + b1 * z_inv + b2 * z_inv^2) / (a0 + a1 * z_inv + a2 * z_inv^2)
  magnitude_db <- 20 * log10(abs(H))
  phase <- atan2(Im(H), Re(H))
  
  return(list(f = f, magnitude_db = magnitude_db, phase = phase))
}

# Plot frequency response
plot_frequency_response <- function(coeffs, fs, title = "Biquad Filter Frequency Response") {
  response <- calculate_frequency_response(coeffs$b0, coeffs$b1, coeffs$b2, 
                                           coeffs$a0, coeffs$a1, coeffs$a2, fs)
  
  par(mfrow = c(2, 1))
  
  # Magnitude plot
  plot(response$f, response$magnitude_db, type = "l", col = "blue",
       xlab = "Frequency (Hz)", ylab = "Magnitude (dB)",
       main = title)
  grid()

  # Phase plot
  plot(response$f, response$phase * 180 / pi, type = "l", col = "red",
       xlab = "Frequency (Hz)", ylab = "Phase (degrees)",
       main = "Phase Response")
  grid()
}

coeffs <- linkwitz_transform_biquad(f0 = 119, q0 = 1.23, fp = 60, qp = 0.7, fs = 48000)
plot_frequency_response(coeffs, fs = 48000, title = "Linkwitz Transform Frequency Response")
print(coeffs)
