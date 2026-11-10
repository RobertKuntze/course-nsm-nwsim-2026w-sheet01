library(tidyverse)
library(ggplot2)
library(dplyr)

wide = read.csv("results.csv", check.names = FALSE)

pairs <- seq(1, ncol(wide), by = 2)

long <- do.call(rbind, lapply(pairs, function(i) {
  data.frame(
    vector = names(wide)[i],
    time   = wide[[i]],
    value  = wide[[i + 1]]
  )
}))

long = na.omit(long)

long$id = as.integer(sub(".*\\[([0-9]+)\\].*", "\\1", long$vector))
long$run = as.integer(sub(".*\\(#([0-9]+).*", "\\1", long$vector))
long$vector = sub("\\s.*", "", long$vector)
long$time = long$time

queue_size = long[long$vector == "queueSizeVector", ]

<<<<<<< HEAD
time_grid = seq(0,7200,by = 1)
=======
time_grid = seq(0,7200,by = 30)
>>>>>>> c8cf910 (Added a cos offset to the timeBetweenCustomers, to simulate customer)

avg_qs = queue_size %>%
    group_by(id) %>%
    group_modify(~ {
        .x %>%
            group_by(run) %>%
            group_modify(~ {
                series = .x %>%
                    group_by(time) %>%
                    summarise(value = mean(value), .groups = "drop") %>%
                    arrange(time)

                tibble(
                    time = time_grid,
                    value = approx(series$time, series$value, xout = time_grid, rule = 1)$y
                )
            }) %>%
            ungroup() %>%
            group_by(time) %>%
            summarise(value = mean(value, na.rm = TRUE), .groups = "drop")
    }) %>%
    ungroup()

avg_qs = na.omit(avg_qs)

total <- avg_qs %>%
  group_by(time) %>%
  summarise(value = sum(value, na.rm = TRUE), .groups = "drop") %>%
  mutate(id = -1)

avg_qs_with_total <- bind_rows(avg_qs, total)

plot_qs = ggplot(
        avg_qs_with_total,
        aes(
            x = time,
            y = value,
            group = factor(id),
            color = factor(id)
        )
) +
geom_line()

dir.create("plots", showWarnings = FALSE)
ggsave("plots/queueSize.png", plot = plot_qs, width = 10, height = 6, dpi = 300)
