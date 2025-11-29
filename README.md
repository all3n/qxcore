# qxcore

A Go core library providing essential utility types and functions.

## Installation

```bash
go get github.com/all3n/qxcore
```

## Features

### Result Type

A generic Result type for handling operations that may fail:

```go
import "github.com/all3n/qxcore/pkg/core"

// Create a successful result
result := core.NewResult(42)
if result.IsOk() {
    value := result.Unwrap()
    // use value
}

// Create an error result
errResult := core.NewResultErr[int](errors.New("something went wrong"))
if errResult.IsErr() {
    // handle error
}

// Use UnwrapOr for default values
value := result.UnwrapOr(0)
```

### Option Type

A generic Option type for representing optional values:

```go
import "github.com/all3n/qxcore/pkg/core"

// Create an Option with a value
opt := core.Some("hello")
if opt.IsSome() {
    value := opt.Unwrap()
    // use value
}

// Create an empty Option
none := core.None[string]()
if none.IsNone() {
    // handle missing value
}

// Use UnwrapOr for default values
value := opt.UnwrapOr("default")
```

## Testing

```bash
go test ./...
```

## License

MIT License
