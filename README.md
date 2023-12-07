Pretty crude sketch that was written in a couple of hours with ChatGPT.

Delay is randomly chosen from min and max values, if you need a static delay just send two same values. 

CSV data it accepts:

## Keyboard

### Send key:
```
1, ASCII code, minDelay, maxDelay
```

### Toggle key:
```
2, ASCII code, pressed(1 or 0), minDelay, maxDelay
```

### Print keys:
```
3, text, minDelay, maxDelay
```

## Mouse

### Mouse click:
```
4, button(1 or 2 or 4), minDelay, maxDelay
```

*1 = left button, 2 = right button, 4 = middle button*

### Mouse toggle:

```
5, button(1 or 2 or 4), pressed(1 or 0), minDelay, maxDelay
```

*1 = left button, 2 = right button, 4 = middle button*

### Mouse move:
```
6, x, y, numSteps, minDelay, maxDelay, minControlDistance, maxControlDistance, offset
```

*minControlDistance, maxControlDistance and offset are responsible for deviation of the movement. Delay values are applied to every numStep, with these 2 values and numStep number you can define the speed.*

