package core

import (
	"errors"
	"testing"
)

func TestVersion(t *testing.T) {
	if Version == "" {
		t.Error("Version should not be empty")
	}
}

func TestResult_NewResult(t *testing.T) {
	result := NewResult(42)
	if !result.IsOk() {
		t.Error("Result should be Ok")
	}
	if result.IsErr() {
		t.Error("Result should not be Err")
	}
	value, err := result.Value()
	if err != nil {
		t.Errorf("Expected no error, got %v", err)
	}
	if value != 42 {
		t.Errorf("Expected 42, got %d", value)
	}
}

func TestResult_NewResultErr(t *testing.T) {
	expectedErr := errors.New("test error")
	result := NewResultErr[int](expectedErr)
	if result.IsOk() {
		t.Error("Result should not be Ok")
	}
	if !result.IsErr() {
		t.Error("Result should be Err")
	}
	_, err := result.Value()
	if err != expectedErr {
		t.Errorf("Expected %v, got %v", expectedErr, err)
	}
}

func TestResult_Unwrap(t *testing.T) {
	result := NewResult("hello")
	if result.Unwrap() != "hello" {
		t.Error("Unwrap should return the value")
	}
}

func TestResult_UnwrapPanics(t *testing.T) {
	defer func() {
		if r := recover(); r == nil {
			t.Error("Unwrap on error Result should panic")
		}
	}()
	result := NewResultErr[int](errors.New("error"))
	result.Unwrap()
}

func TestResult_UnwrapOr(t *testing.T) {
	result := NewResult(10)
	if result.UnwrapOr(20) != 10 {
		t.Error("UnwrapOr should return the value when Ok")
	}

	errResult := NewResultErr[int](errors.New("error"))
	if errResult.UnwrapOr(20) != 20 {
		t.Error("UnwrapOr should return the default when Err")
	}
}

func TestOption_Some(t *testing.T) {
	opt := Some(42)
	if !opt.IsSome() {
		t.Error("Option should be Some")
	}
	if opt.IsNone() {
		t.Error("Option should not be None")
	}
	if opt.Unwrap() != 42 {
		t.Errorf("Expected 42, got %d", opt.Unwrap())
	}
}

func TestOption_None(t *testing.T) {
	opt := None[int]()
	if opt.IsSome() {
		t.Error("Option should not be Some")
	}
	if !opt.IsNone() {
		t.Error("Option should be None")
	}
}

func TestOption_UnwrapPanics(t *testing.T) {
	defer func() {
		if r := recover(); r == nil {
			t.Error("Unwrap on None should panic")
		}
	}()
	opt := None[int]()
	opt.Unwrap()
}

func TestOption_UnwrapOr(t *testing.T) {
	someOpt := Some("value")
	if someOpt.UnwrapOr("default") != "value" {
		t.Error("UnwrapOr should return the value when Some")
	}

	noneOpt := None[string]()
	if noneOpt.UnwrapOr("default") != "default" {
		t.Error("UnwrapOr should return the default when None")
	}
}
