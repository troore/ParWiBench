% one second
t = 0: 1/1e3: 0.999

% Ten random
a = 2 * randint(1, 10, 2) - 1;

% Ten rectangular waveform
g = ones(1, 100);
g = [g, g, g, g, g, g, g, g, g, g];

% Binary amplitude modulation
s = a(ceil(10 * t + 0.01)) .* g .* cos(2 * pi * 100 * t);

% Drawing
subplot(2, 1, 1);
plot(t, a(ceil(10 * t + 0.01)));
axis([0, 1, -1.2, 1.2]);
subplot(2, 1, 2);
plot(t, s);