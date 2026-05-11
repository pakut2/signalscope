import ctypes
import time
import tkinter

import audio

BAR_COLOR = "#00ff88"
BG_COLOR = "#171717"

BAR_SPACING = 2
SMOOTHNESS = 8.0

FRAME_MS = 16


class App:
    def __init__(self, root):
        spectrum_analyzer, audio_decoder, OnSamplesProcessed = audio.init_audio()
        self.spectrum_analyzer = spectrum_analyzer
        self.audio_decoder = audio_decoder
        self.interpolated_frequencies = []
        self.last_frame_timestamp = time.monotonic()

        root.title("SignalScope")
        root.configure(bg=BG_COLOR)
        root.geometry("1248x768")
        root.protocol("WM_DELETE_WINDOW", self.on_close)

        self.canvas = tkinter.Canvas(root, bg=BG_COLOR, highlightthickness=0)
        self.canvas.pack(fill=tkinter.BOTH, expand=True)

        spectrum_analyzer.spectrum_analyzer_init()

        @OnSamplesProcessed
        def on_samples_processed(samples, sample_count):
            spectrum_analyzer.spectrum_samples_append(samples, sample_count)

        self.on_samples_processed = on_samples_processed  # Prevent GC cleanup

        self.decoder = audio_decoder.audio_decoder_start(
            audio.DEVICE_TYPE.SPEAKER.value, on_samples_processed
        )
        self.sample_rate = self.decoder.sample_rate

        self.canvas.after(0, self.render)

    def render(self):
        current_frame_timestamp = time.monotonic()
        frame_elapsed_seconds = current_frame_timestamp - self.last_frame_timestamp
        self.last_frame_timestamp = current_frame_timestamp

        spectrum = self.spectrum_analyzer.spectrum_create(self.sample_rate)

        if spectrum.frequency_bin_count > len(self.interpolated_frequencies):
            self.interpolated_frequencies.extend(
                [0.0]
                * (spectrum.frequency_bin_count - len(self.interpolated_frequencies))
            )

        self.canvas.delete("all")

        window_width = self.canvas.winfo_width()
        window_height = self.canvas.winfo_height()

        if spectrum.frequency_bin_count > 0 and window_width > 0 and window_height > 0:
            for i in range(spectrum.frequency_bin_count):
                self.interpolated_frequencies[i] += (
                    (
                        spectrum.normalized_frequencies[i]
                        - self.interpolated_frequencies[i]
                    )
                    * frame_elapsed_seconds
                    * SMOOTHNESS
                )

                bar_width = max(
                    window_width // (spectrum.frequency_bin_count * BAR_SPACING), 1
                )

                bar_start_x = i * BAR_SPACING * bar_width
                bar_end_x = bar_start_x + bar_width
                if bar_end_x > window_width:
                    break

                bar_height = self.interpolated_frequencies[i] * window_height

                self.canvas.create_rectangle(
                    bar_start_x,
                    window_height - bar_height,
                    bar_end_x,
                    window_height,
                    fill=BAR_COLOR,
                    outline="",
                )

        self.spectrum_analyzer.spectrum_destroy(ctypes.byref(spectrum))

        self.canvas.after(FRAME_MS, self.render)

    def on_close(self):
        self.spectrum_analyzer.audio_decoder_stop(
            audio.DEVICE_TYPE.SPEAKER.value, self.decoder.device
        )

        self.canvas.winfo_toplevel().destroy()


if __name__ == "__main__":
    root = tkinter.Tk()
    App(root)

    root.mainloop()
