// Package core provides core utility functions and types for qxcore.
package core

// Version is the current version of qxcore.
const Version = "0.1.0"

// Result represents a generic result type that can hold either a value or an error.
type Result[T any] struct {
	value T
	err   error
}

// NewResult creates a new Result with the given value.
func NewResult[T any](value T) Result[T] {
	return Result[T]{value: value}
}

// NewResultErr creates a new Result with the given error.
func NewResultErr[T any](err error) Result[T] {
	var zero T
	return Result[T]{value: zero, err: err}
}

// Value returns the value and error from the Result.
func (r Result[T]) Value() (T, error) {
	return r.value, r.err
}

// IsOk returns true if the Result contains a value (no error).
func (r Result[T]) IsOk() bool {
	return r.err == nil
}

// IsErr returns true if the Result contains an error.
func (r Result[T]) IsErr() bool {
	return r.err != nil
}

// Unwrap returns the value, panicking if there is an error.
func (r Result[T]) Unwrap() T {
	if r.err != nil {
		panic(r.err)
	}
	return r.value
}

// UnwrapOr returns the value or the provided default if there is an error.
func (r Result[T]) UnwrapOr(defaultValue T) T {
	if r.err != nil {
		return defaultValue
	}
	return r.value
}

// Option represents an optional value.
type Option[T any] struct {
	value   T
	present bool
}

// Some creates an Option containing a value.
func Some[T any](value T) Option[T] {
	return Option[T]{value: value, present: true}
}

// None creates an empty Option.
func None[T any]() Option[T] {
	return Option[T]{present: false}
}

// IsSome returns true if the Option contains a value.
func (o Option[T]) IsSome() bool {
	return o.present
}

// IsNone returns true if the Option is empty.
func (o Option[T]) IsNone() bool {
	return !o.present
}

// Unwrap returns the value, panicking if the Option is empty.
func (o Option[T]) Unwrap() T {
	if !o.present {
		panic("called Unwrap on an empty Option")
	}
	return o.value
}

// UnwrapOr returns the value or the provided default if the Option is empty.
func (o Option[T]) UnwrapOr(defaultValue T) T {
	if !o.present {
		return defaultValue
	}
	return o.value
}
