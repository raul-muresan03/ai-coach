"use client"

import { Button } from "./button"

export const HeroSection = () => {
    return (
        <div>
            <div className="h-screen flex items-center justify-center">

                <div className="flex flex-col items-center space-y-10">
                    <div className="font-bold text-6xl">
                        Welcome to -appname-
                    </div>
                    <div className="text-4xl">
                        <div className="flex items-center justify-center">
                            Monitor your fitness progress&nbsp;
                            <strong> without the scale</strong>!
                        </div>
                        <div className="flex items-center justify-center">
                            Use AI to estimate body fat percentage from photos or enter
                        </div>
                        <div className="flex items-center justify-center">
                            your biometric data for accurate analysis. Simple, private and efficient.
                        </div>
                    </div>
                    <div>
                        <img src="/hero.png" width={500} height={200}/>
                    </div>
                    <div>
                        <Button>
                            Get -appname- now
                        </Button>
                    </div>
                </div>
            </div>

        </div>
    )
}