# time

## structs
### PuleTimestamp
milliseconds since epoch
```c
struct {
  valueUnixTs : int64_t;
};
```
### PuleNanosecond
```c
struct {
  valueNano : int64_t;
};
```
### PuleMicrosecond
```c
struct {
  valueMicro : int64_t;
};
```
### PuleMillisecond
```c
struct {
  valueMilli : int64_t;
};
```

## functions
### puleMicrosecond
```c
puleMicrosecond(
  value : int64_t
) PuleMicrosecond;
```
### puleSleepMicrosecond
```c
puleSleepMicrosecond(
  us : PuleMicrosecond
) void;
```
### puleTimestampNow
```c
puleTimestampNow() PuleTimestamp;
```
