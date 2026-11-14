library(tidyverse)

wide <- read.csv("results.csv", check.names = FALSE)


long <- map_dfr(seq(1, ncol(wide), by = 2), \(i) tibble(
    name  = names(wide)[i],
    time  = wide[[i]],
    value = wide[[i + 1]]
  )) %>%
  drop_na() %>%
  mutate(
    vector = str_extract(name, "^\\S+"),
    id     = as.integer(str_match(name, "\\[(\\d+)\\]")[, 2]),
    run    = as.integer(str_match(name, "\\(#(\\d+)")[, 2]),
    bin    = floor(time / 90) * 90
  )

# average once for every vector: within runs first, then across runs
avg <- long %>%
  summarise(value = mean(value), .by = c(vector, id, run, bin)) %>%
  summarise(value = mean(value), .by = c(vector, id, bin))

# total over modules, per vector
total <- avg %>%
  summarise(value = sum(value), .by = c(vector, bin)) %>%
  mutate(id = -1L)

avg_all <- bind_rows(avg, total) %>%
  mutate(series = if_else(id == -1, "Total", paste0("tic[", id, "]")))

customer_wait <- long %>%
  filter(str_starts(vector, "customerWait"))

# filter afterwards, one plot per vector
plot_vector <- function(vec, unit = "(s)", file = paste0("plots/", vec, ".png")) {
  p <- avg_all %>%
    filter(vector == vec) %>%
    ggplot(aes(bin, value, color = series)) +
    geom_line() +
    labs(title = vec, x = "Time (s)", y = paste(vec, unit), color = NULL)
  ggsave(file, p, width = 10, height = 6, dpi = 300)
  p
}

dir.create("plots", showWarnings = FALSE)
plot_vector("queueSizeVector", unit = "")
plot_vector("idleTimeVector")
plot_vector("serviceTimeVector")

cw_ecdf <- ggplot(customer_wait, aes(value, color = factor(id))) +
  stat_ecdf() +
  stat_ecdf(aes(color = NULL), color = "black", linewidth = 1) +
  labs(x = "Customer wait time (s)", y = "P(wait ≤ x)", color = "id")
ggsave("plots/customerWait_ecdf.png", cw_ecdf)
# or all of them at once:
# walk(unique(avg_all$vector), plot_vector)
